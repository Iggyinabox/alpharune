/// @file test_rules_compliance.cpp
/// Engine-level tests pinning behaviour to specific Core Rules, written while
/// auditing the engine against the 2026-10-09 rules (docs/rules-audit-2026-10.md).
/// Each test names the rule it enforces.

#include "cards/card_test_fixture.h"
#include "engine/effect_executor.h"
#include "engine/game_engine.h"

#include <gtest/gtest.h>

namespace riftbound::test {
namespace {

class RulesComplianceTest : public CardTestFixture {
protected:
    std::unique_ptr<GameEngine> engine_;
    FirstChoiceAgent a1_, a2_;
    GameState& s() { return engine_->mutableState(); }

    void makeEngine(int n_bfs = 2) {
        engine_ = std::make_unique<GameEngine>(card_db, events, card_registry);
        auto& st = s();
        st.mode = ModeOfPlay{};
        st.players[0].id = P1;
        st.players[1].id = P2;
        for (int i = 0; i < n_bfs; ++i) {
            BattlefieldState bf;
            bf.id = static_cast<int>(st.battlefields.size());
            st.battlefields.push_back(bf);
        }
        engine_->testHook_setAgents(&a1_, &a2_);
        st.turn.turn_player = P1;
        st.turn.phase = TurnPhase::MainPhase;
    }

    /// A legend in `p`'s Legend Zone (def_id may be kInvalidId for a blank one).
    GameObjectId setLegend(PlayerId p, CardDefId def_id = kInvalidId) {
        auto id = s().createObject();
        auto& lo = s().getObject(id);
        lo.owner = lo.controller = p;
        lo.card_type = CardType::Legend;
        lo.card_def_id = def_id;
        if (def_id != kInvalidId) lo.name = card_db.get(def_id).name;
        lo.zone = ZoneType::LegendZone;
        s().player(p).legend_zone = id;
        return id;
    }

    GameObjectId engineUnit(PlayerId p, int might, int at_bf = -1,
                            CardDefId def_id = kInvalidId) {
        auto id = s().createObject();
        auto& u = s().getObject(id);
        u.owner = u.controller = p;
        u.card_type = CardType::Unit;
        u.card_def_id = def_id;
        u.name = def_id != kInvalidId ? card_db.get(def_id).name : "TestUnit";
        u.base_might = u.current_might = might;
        if (at_bf >= 0) {
            u.zone = ZoneType::BattlefieldZone;
            u.location = BattlefieldLocation{static_cast<BattlefieldId>(at_bf)};
        } else {
            u.zone = ZoneType::Base;
            u.location = BaseLocation{p};
        }
        return id;
    }

    bool onBoard(GameObjectId id) { return s().getObject(id).location.has_value(); }
};

// ─── Death replacement effects ("would die ... instead") ────────────────────
// A replacement applies only under its own printed condition. The Boss (264):
// "When a buffed unit you control would die, you may pay ... exhaust me, and
// spend its buff to ... recall it instead."

constexpr CardDefId kTheBoss = 264;

TEST_F(RulesComplianceTest, DeathReplacement_TheBoss_DoesNotSaveAnUnbuffedUnit) {
    makeEngine();
    setLegend(P1, kTheBoss);
    setLegend(P2);
    auto unit = engineUnit(P1, 2, 0);
    s().getObject(unit).damage_marked = 2;      // lethal

    engine_->testHook_processLethalDamage();

    EXPECT_FALSE(onBoard(unit)) << "The Boss only saves BUFFED units";
    EXPECT_EQ(s().getObject(unit).zone, ZoneType::Trash);
}

TEST_F(RulesComplianceTest, DeathReplacement_TheBoss_SavesABuffedUnit) {
    makeEngine();
    auto boss = setLegend(P1, kTheBoss);
    setLegend(P2);
    auto unit = engineUnit(P1, 2, 0);
    s().getObject(unit).buff_count = 1;
    s().getObject(unit).recomputeMight();
    s().getObject(unit).damage_marked = 5;      // lethal

    engine_->testHook_processLethalDamage();

    EXPECT_TRUE(onBoard(unit));
    EXPECT_TRUE(s().getObject(unit).isAtBase()) << "recalled to base";
    EXPECT_EQ(s().getObject(unit).buff_count, 0) << "its buff is spent";
    EXPECT_TRUE(s().getObject(boss).is_exhausted) << "The Boss exhausts to pay";
}

TEST_F(RulesComplianceTest, DeathReplacement_AppliesToEffectKillsToo) {
    // A unit killed by a spell or ability "would die" just the same, so the
    // replacement must be consulted on that path as well.
    makeEngine();
    setLegend(P1, kTheBoss);
    setLegend(P2);
    auto unit = engineUnit(P1, 2, 0);
    s().getObject(unit).buff_count = 1;
    s().getObject(unit).recomputeMight();

    EffectExecutor exec(s(), events, card_db, &card_registry);
    exec.killObject(unit);

    EXPECT_TRUE(onBoard(unit)) << "saved by The Boss";
    EXPECT_TRUE(s().getObject(unit).isAtBase());
}

TEST_F(RulesComplianceTest, DeathReplacement_ExhaustedBossCannotSaveASecondUnit) {
    makeEngine();
    auto boss = setLegend(P1, kTheBoss);
    setLegend(P2);
    s().getObject(boss).is_exhausted = true;
    auto unit = engineUnit(P1, 2, 0);
    s().getObject(unit).buff_count = 1;
    s().getObject(unit).recomputeMight();
    s().getObject(unit).damage_marked = 5;

    engine_->testHook_processLethalDamage();

    EXPECT_FALSE(onBoard(unit)) << "the cost (exhaust me) can't be paid";
}

// ─── CR 467.2.c.5 — replacement effects apply to combat damage ASSIGNMENT ───

namespace {
int assignedTo(const std::vector<DamageAssignment>& da, GameObjectId id) {
    for (const auto& a : da) if (a.target_unit == id) return a.damage;
    return 0;
}
}  // namespace

TEST_F(RulesComplianceTest, CombatDamage_PreventionRaisesTheLethalAssignment) {
    // "Prevent the next 7 damage" (Ki Barrier): 467.2.c.4.a — the assigner must
    // assign enough that the unit actually takes lethal damage.
    makeEngine();
    setLegend(P1); setLegend(P2);
    auto shielded = engineUnit(P2, 3, 0);
    auto other    = engineUnit(P2, 2, 0);
    s().getObject(shielded).prevent_damage_remaining = 7;

    auto adv = engine_->testHook_advanceCombatDamage(P1, 12, {shielded, other});
    ASSERT_FALSE(adv.legal.empty());
    const auto& da = adv.legal.front().damage_assignments;
    EXPECT_EQ(assignedTo(da, shielded), 10) << "3 lethal + 7 prevented";
    EXPECT_EQ(assignedTo(da, other), 2);

    engine_->testHook_executeIntent(adv.legal.front());
    EXPECT_EQ(s().getObject(shielded).damage_marked, 3) << "7 of the 10 was prevented";
    EXPECT_EQ(s().getObject(shielded).prevent_damage_remaining, 0);
    EXPECT_EQ(s().getObject(other).damage_marked, 2);
}

TEST_F(RulesComplianceTest, CombatDamage_PreventionAbsorbsAnInsufficientAssignment) {
    makeEngine();
    setLegend(P1); setLegend(P2);
    auto shielded = engineUnit(P2, 3, 0);
    s().getObject(shielded).prevent_damage_remaining = 7;

    auto adv = engine_->testHook_advanceCombatDamage(P1, 5, {shielded});
    ASSERT_FALSE(adv.legal.empty());
    engine_->testHook_executeIntent(adv.legal.front());

    EXPECT_EQ(s().getObject(shielded).damage_marked, 0) << "all 5 prevented";
    EXPECT_EQ(s().getObject(shielded).prevent_damage_remaining, 2);
}

TEST_F(RulesComplianceTest, CombatDamage_UnitThatCannotBeDamagedIsExempt) {
    // CR 467.2.c.10: no amount is lethal for it, so it is skipped and the
    // damage goes to the units that can take it.
    makeEngine();
    setLegend(P1); setLegend(P2);
    auto immune = engineUnit(P2, 3, 0);
    auto other  = engineUnit(P2, 2, 0);
    s().getObject(immune).immune_to_damage = true;

    auto adv = engine_->testHook_advanceCombatDamage(P1, 3, {immune, other});
    ASSERT_FALSE(adv.legal.empty());
    const auto& da = adv.legal.front().damage_assignments;
    EXPECT_EQ(assignedTo(da, immune), 0);
    EXPECT_EQ(assignedTo(da, other), 3);

    engine_->testHook_executeIntent(adv.legal.front());
    EXPECT_EQ(s().getObject(immune).damage_marked, 0);
    EXPECT_EQ(s().getObject(other).damage_marked, 3);
}

TEST_F(RulesComplianceTest, CombatDamage_NextDamagePreventedIsNotDealtInCombat) {
    // "The next time that unit would be dealt damage this turn, prevent it."
    makeEngine();
    setLegend(P1); setLegend(P2);
    auto guarded = engineUnit(P2, 3, 0);
    s().getObject(guarded).prevent_next_damage_this_turn = true;

    Intent assign;
    assign.type = IntentType::AssignCombatDamage;
    assign.player = P1;
    assign.damage_assignments = {{guarded, 4}};
    engine_->testHook_executeIntent(assign);

    EXPECT_EQ(s().getObject(guarded).damage_marked, 0);
    EXPECT_FALSE(s().getObject(guarded).prevent_next_damage_this_turn) << "consumed";
}

TEST_F(RulesComplianceTest, CombatDamage_DoubledDamageNeedsHalfTheAssignment) {
    // Damage to this unit is doubled this turn: 467.2.c.4.a — assign the
    // minimum that results in lethal damage.
    makeEngine();
    setLegend(P1); setLegend(P2);
    auto marked = engineUnit(P2, 4, 0);
    auto other  = engineUnit(P2, 2, 0);
    s().getObject(marked).damage_doubled_this_turn = true;

    auto adv = engine_->testHook_advanceCombatDamage(P1, 4, {marked, other});
    ASSERT_FALSE(adv.legal.empty());
    const auto& da = adv.legal.front().damage_assignments;
    EXPECT_EQ(assignedTo(da, marked), 2) << "2 assigned is 4 dealt";
    EXPECT_EQ(assignedTo(da, other), 2);

    engine_->testHook_executeIntent(adv.legal.front());
    EXPECT_EQ(s().getObject(marked).damage_marked, 4);
}

// ─── CR 190.3 / 323.7-323.10 — Contested status ────────────────────────────
// These run a Cleanup in a Closed State so the staged Showdown / Combat is
// recorded but not started (323.11 / 323.12 need a Neutral Open State).

TEST_F(RulesComplianceTest, Contested_RemovedWhenContesterHasNoUnitsThere) {
    // P1 moved in and contested P2's battlefield, then P1's unit left (died /
    // bounced) before the showdown opened. CR 323.10: Contested is removed.
    makeEngine();
    setLegend(P1); setLegend(P2);
    s().turn.oc_state = OpenClosedState::Closed;
    engineUnit(P2, 3, 0);
    auto& bf = s().battlefields[0];
    bf.controller = P2;
    bf.is_contested = true;
    bf.contested_by = P1;
    bf.showdown_staged = true;

    engine_->testHook_cleanup();

    EXPECT_FALSE(bf.is_contested);
    EXPECT_FALSE(bf.showdown_staged) << "no showdown for a contest that no longer exists";
    EXPECT_FALSE(bf.combat_staged);
    ASSERT_TRUE(bf.controller.has_value());
    EXPECT_EQ(*bf.controller, P2);
}

TEST_F(RulesComplianceTest, Contested_AppliedWhenAUnitArrivesByAnEffect) {
    // CR 190.3.a: a Unit that "otherwise becomes present" at a battlefield its
    // controller doesn't control applies Contested — not only a standard move.
    makeEngine();
    setLegend(P1); setLegend(P2);
    s().turn.oc_state = OpenClosedState::Closed;
    engineUnit(P2, 3, 0);
    auto& bf = s().battlefields[0];
    bf.controller = P2;
    engineUnit(P1, 2, 0);            // put there by an effect: no move intent ran

    engine_->testHook_cleanup();

    EXPECT_TRUE(bf.is_contested);
    EXPECT_EQ(bf.contested_by, P1);
    EXPECT_TRUE(bf.combat_staged) << "CR 323.8: opposing units -> Combat staged";
}

TEST_F(RulesComplianceTest, Contested_NotAppliedByTheControllersOwnUnits) {
    makeEngine();
    setLegend(P1); setLegend(P2);
    s().turn.oc_state = OpenClosedState::Closed;
    engineUnit(P2, 3, 0);
    auto& bf = s().battlefields[0];
    bf.controller = P2;

    engine_->testHook_cleanup();

    EXPECT_FALSE(bf.is_contested);
    EXPECT_FALSE(bf.showdown_staged);
}

TEST_F(RulesComplianceTest, Contested_ReappliedByRemainingNonControllerUnits) {
    // CR 323.10.a: Contested (by P1) is removed because P1 has no units, but
    // P2 has units at a battlefield P2 does not control -> P2 applies it.
    makeEngine();
    setLegend(P1); setLegend(P2);
    s().turn.oc_state = OpenClosedState::Closed;
    engineUnit(P2, 3, 0);
    auto& bf = s().battlefields[0];
    bf.controller = std::nullopt;     // uncontrolled
    bf.is_contested = true;
    bf.contested_by = P1;

    engine_->testHook_cleanup();

    EXPECT_TRUE(bf.is_contested);
    EXPECT_EQ(bf.contested_by, P2);
    EXPECT_TRUE(bf.showdown_staged) << "CR 323.7";
    EXPECT_FALSE(bf.combat_staged);
}

// ─── CR 164.2 — paying Energy and Power with runes ─────────────────────────
// A Basic Rune has "[E]: Add [1]" and "Recycle this: Add [C]". Recycling does
// not need the rune to be exhausted, and a rune exhausted for Energy can still
// be recycled for Power.

namespace {
constexpr CardDefId kPunchFirst = 419;   // spell, [1] + 2 Body power
}  // namespace

class CostPaymentRulesTest : public RulesComplianceTest {
protected:
    GameObjectId rune(PlayerId p, Domain d, bool exhausted = false) {
        auto id = s().createObject();
        auto& r = s().getObject(id);
        r.owner = r.controller = p;
        r.card_type = CardType::Rune;
        r.name = "Rune";
        r.domains = {d};
        r.zone = ZoneType::Base;
        r.location = BaseLocation{p};
        r.is_exhausted = exhausted;
        return id;
    }
    GameObjectId cardInHand(PlayerId p, CardDefId def_id) {
        auto id = s().createObject();
        auto& c = s().getObject(id);
        const auto& def = card_db.get(def_id);
        c.owner = c.controller = p;
        c.card_def_id = def_id;
        c.card_type = def.card_type;
        c.name = def.name;
        c.domains = def.domains;
        c.keywords = def.keywords;
        c.zone = ZoneType::Hand;
        s().player(p).hand.push_back(id);
        return id;
    }
    int runesInBase(PlayerId p) {
        int n = 0;
        for (auto& [id, o] : s().objects)
            if (o.isRune() && o.controller == p && o.location.has_value()) ++n;
        return n;
    }
};

TEST_F(CostPaymentRulesTest, BaseCost_TwoReadyRunesPayOneEnergyAndTwoPower) {
    // [1] + [O][O] with exactly two ready Body runes: exhaust one for the
    // Energy, then recycle both for the Power.
    makeEngine();
    setLegend(P1); setLegend(P2);
    rune(P1, Domain::Body);
    rune(P1, Domain::Body);
    auto spell = cardInHand(P1, kPunchFirst);

    EXPECT_TRUE(engine_->testHook_canAfford(P1, spell));
    EXPECT_TRUE(engine_->testHook_payCardCost(P1, spell));
    EXPECT_EQ(runesInBase(P1), 0) << "both runes recycled for Power";
}

TEST_F(CostPaymentRulesTest, BaseCost_NotAffordableWithoutEnoughMatchingRunes) {
    makeEngine();
    setLegend(P1); setLegend(P2);
    rune(P1, Domain::Body);
    rune(P1, Domain::Fury);          // wrong domain for the second Power
    auto spell = cardInHand(P1, kPunchFirst);

    EXPECT_FALSE(engine_->testHook_canAfford(P1, spell));
}

TEST_F(CostPaymentRulesTest, BaseCost_SpendsFloatingPowerBeforeRecyclingRunes) {
    makeEngine();
    setLegend(P1); setLegend(P2);
    rune(P1, Domain::Fury);          // pays the Energy only
    s().player(P1).rune_pool.power[static_cast<int>(Domain::Body)] = 2;
    auto spell = cardInHand(P1, kPunchFirst);

    ASSERT_TRUE(engine_->testHook_canAfford(P1, spell));
    EXPECT_TRUE(engine_->testHook_payCardCost(P1, spell));
    EXPECT_EQ(s().player(P1).rune_pool.power[static_cast<int>(Domain::Body)], 0);
    EXPECT_EQ(runesInBase(P1), 1) << "no rune recycled: the floating Power paid";
}

TEST_F(CostPaymentRulesTest, AdditionalCost_AnyDomainPowerMayMixDomains) {
    // [A][A] is two Power of any Domain; they need not match each other.
    makeEngine();
    setLegend(P1); setLegend(P2);
    rune(P1, Domain::Fury);
    rune(P1, Domain::Calm);

    EXPECT_TRUE(engine_->testHook_canPayAdditionalCost(P1, 0, 2, Domain::Count));
    EXPECT_TRUE(engine_->testHook_payAdditionalCost(P1, 0, 2, Domain::Count));
    EXPECT_EQ(runesInBase(P1), 0);
}

TEST_F(CostPaymentRulesTest, AdditionalCost_OneRunePaysEnergyAndPower) {
    makeEngine();
    setLegend(P1); setLegend(P2);
    rune(P1, Domain::Fury);

    EXPECT_TRUE(engine_->testHook_canPayAdditionalCost(P1, 1, 1, Domain::Fury));
    EXPECT_TRUE(engine_->testHook_payAdditionalCost(P1, 1, 1, Domain::Fury));
    EXPECT_EQ(runesInBase(P1), 0);
}

TEST_F(CostPaymentRulesTest, AdditionalCost_ExhaustedRuneCannotPayEnergy) {
    makeEngine();
    setLegend(P1); setLegend(P2);
    rune(P1, Domain::Fury, /*exhausted=*/true);

    EXPECT_FALSE(engine_->testHook_canPayAdditionalCost(P1, 1, 0, Domain::Fury));
    EXPECT_TRUE(engine_->testHook_canPayAdditionalCost(P1, 0, 1, Domain::Fury))
        << "but it can still be recycled for Power";
}

TEST_F(CostPaymentRulesTest, AdditionalCost_FailedPaymentChangesNothing) {
    makeEngine();
    setLegend(P1); setLegend(P2);
    auto r = rune(P1, Domain::Fury);

    EXPECT_FALSE(engine_->testHook_payAdditionalCost(P1, 2, 0, Domain::Fury));
    EXPECT_FALSE(s().getObject(r).is_exhausted) << "nothing is paid when it can't be paid in full";
}

}  // namespace
}  // namespace riftbound::test
