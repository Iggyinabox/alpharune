/// @file test_wave_b_combat.cpp
/// Wave B combat-resolution cluster:
///   Symbol of the Solari (227) — attacker-tie recalls ALL units
///     (combatResolutionStep is private; we verify the card sets the
///      PlayerState flag the engine's tie branch reads.)

#include "tests/cards/card_test_fixture.h"
#include <gtest/gtest.h>

namespace riftbound::test {
namespace {

using CombatTest = CardTestFixture;

constexpr CardDefId kSymbolSolari = 227;
constexpr CardDefId kReckonerArena = 281;
constexpr CardDefId kArachnoidHorror = 679;  // WhenIConquerOrHold -> +2 XP

TEST_F(CombatTest, SymbolOfTheSolari_SetsRecallAllFlag) {
    auto sym = addUnit(P1, kSymbolSolari, 0, /*at_bf=*/-1);
    state.getObject(sym).card_type = CardType::Gear;
    EXPECT_FALSE(state.player(P1).recall_all_on_attacker_tie);
    card_registry.get(kSymbolSolari)->applyPassiveAura(state, P1);
    EXPECT_TRUE(state.player(P1).recall_all_on_attacker_tie)
        << "Symbol of the Solari arms attacker-tie recall-all for its controller";
}

// ── Reckoner's Arena: on hold, re-activates units' conquer effects here ──
TEST_F(CombatTest, ReckonersArena_ReactivatesConquerEffectsHere) {
    // Make battlefield 0's card object a Reckoner's Arena.
    auto arena = addUnit(P1, kReckonerArena, 0, /*at_bf=*/-1);
    state.getObject(arena).card_type = CardType::Battlefield;
    state.battlefields[0].card_object_id = arena;
    // A holder unit at bf 0 whose conquer effect gains 2 XP.
    addUnit(P1, kArachnoidHorror, 4, /*at_bf=*/0);
    int before = state.player(P1).xp;

    EffectExecutor exec(state, events, card_db);
    CardContext ctx{state, events, exec, P1, arena};
    ctx.firing_trigger = TriggerType::WhenYouHoldHere;
    ctx.registry = &card_registry;
    card_registry.get(kReckonerArena)->onTrigger(ctx, {});
    // CR 383.4.g.1: an "activated" conquer effect "is placed on the chain as if
    // it had just triggered" — it is the unit's own chain item, not something
    // run inside the Arena's ability (this used to expect the XP at once).
    EXPECT_EQ(state.player(P1).xp, before) << "nothing has resolved yet";
    ASSERT_EQ(state.chain.items.size(), 1u);
    const ChainItem queued = state.chain.items.back();
    EXPECT_TRUE(queued.is_ability);
    EXPECT_FALSE(queued.is_activated_ability);
    EXPECT_EQ(queued.card_def_id, kArachnoidHorror);
    // Resolve it the way the chain would.
    state.chain.items.pop_back();
    state.chain.resuming = queued;
    CardContext unit_ctx{state, events, exec, queued.controller, queued.source};
    unit_ctx.firing_trigger = queued.fired_trigger;
    unit_ctx.registry = &card_registry;
    card_registry.get(queued.card_def_id)->onTrigger(unit_ctx, {});
    state.chain.resuming.reset();
    EXPECT_EQ(state.player(P1).xp, before + 2)
        << "Reckoner's Arena activates the unit's conquer effect (+2 XP)";
}

}  // namespace
}  // namespace riftbound::test
