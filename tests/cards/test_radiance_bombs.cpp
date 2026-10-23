/// @file test_radiance_bombs.cpp
/// Radiance Bomb support: [Deploy] gear, gear [Deathknell], Detonation
/// Amplifier's battlefield-scoped extra fires, the Bomb token's effect,
/// Ziggs, Short-Fused's bonus damage and Explosimian's hold.

#include "card_test_fixture.h"

#include "cards/card_helpers.h"
#include "cards/card_registry.h"
#include "effects/effect_types.h"
#include "engine/effect_executor.h"
#include "engine/trigger_manager.h"

#include <gtest/gtest.h>

namespace riftbound::test {

namespace {
constexpr CardDefId kBomb = 1102;
constexpr CardDefId kAmplifier = 970;
constexpr CardDefId kGoodStuff = 1044;
constexpr CardDefId kAnvil = 965;
constexpr CardDefId kShortFused = 975;
constexpr CardDefId kExplosimian = 1101;
}  // namespace

class RadianceBombTest : public CardTestFixture {
protected:
    GameObjectId addGearAt(PlayerId owner, CardDefId def_id, int at_bf) {
        auto id = state.createObject();
        auto& obj = state.getObject(id);
        obj.owner = owner;
        obj.controller = owner;
        obj.card_def_id = def_id;
        obj.card_type = CardType::Gear;
        obj.name = card_db.get(def_id).name;
        obj.zone = ZoneType::BattlefieldZone;
        obj.location = BattlefieldLocation{static_cast<BattlefieldId>(at_bf)};
        if (def_id == kBomb) { obj.super_type = SuperType::Token; obj.is_token = true; }
        return id;
    }
    // Mark a gear dead at its battlefield, as killObject would.
    LocationId markDead(GameObjectId id) {
        auto& obj = state.getObject(id);
        LocationId was = *obj.location;
        obj.last_location = obj.location;
        obj.location = std::nullopt;
        obj.zone = ZoneType::Banishment;
        return was;
    }
    int chainSize() const { return static_cast<int>(state.chain.items.size()); }
    bool onBoard(GameObjectId id) const {
        return state.objectExists(id) && state.getObject(id).location.has_value();
    }
};

TEST_F(RadianceBombTest, DeployFlags) {
    EXPECT_TRUE(card_registry.get(kBomb)->hasDeploy());
    EXPECT_TRUE(card_registry.get(kAmplifier)->hasDeploy());
    EXPECT_TRUE(card_registry.get(kGoodStuff)->hasDeploy());
    EXPECT_FALSE(card_registry.get(kAnvil)->hasDeploy());
}

TEST_F(RadianceBombTest, GearDeathknell_BombFiresOnce) {
    auto bomb = addGearAt(P1, kBomb, 0);
    ChainManager cm(state, events, card_db);
    TriggerManager tm(state, events, card_db, cm, card_registry);
    tm.subscribe();
    auto was = markDead(bomb);
    events.emit(LeftBoardEvent{bomb, P1, CardType::Gear, was, ZoneType::Banishment, true});
    EXPECT_EQ(chainSize(), 1);
}

TEST_F(RadianceBombTest, Amplifier_DoublesDeathknellOnlyAtItsBattlefield) {
    addGearAt(P1, kAmplifier, 0);
    auto here = addGearAt(P1, kBomb, 0);
    auto there = addGearAt(P1, kBomb, 1);
    ChainManager cm(state, events, card_db);
    TriggerManager tm(state, events, card_db, cm, card_registry);
    tm.subscribe();

    auto was = markDead(here);
    events.emit(LeftBoardEvent{here, P1, CardType::Gear, was, ZoneType::Banishment, true});
    EXPECT_EQ(chainSize(), 2) << "Bomb at the Amplifier's battlefield fires twice";

    state.chain.items.clear();
    was = markDead(there);
    events.emit(LeftBoardEvent{there, P1, CardType::Gear, was, ZoneType::Banishment, true});
    EXPECT_EQ(chainSize(), 1) << "Bomb at another battlefield fires once";
}

TEST_F(RadianceBombTest, Amplifier_DoesNotDoubleOpponentsDeathknell) {
    addGearAt(P2, kAmplifier, 0);
    auto bomb = addGearAt(P1, kBomb, 0);
    ChainManager cm(state, events, card_db);
    TriggerManager tm(state, events, card_db, cm, card_registry);
    tm.subscribe();
    auto was = markDead(bomb);
    events.emit(LeftBoardEvent{bomb, P1, CardType::Gear, was, ZoneType::Banishment, true});
    EXPECT_EQ(chainSize(), 1);
}

TEST_F(RadianceBombTest, Deploy_OpponentHoldKillsBomb) {
    auto bomb = addGearAt(P1, kBomb, 0);
    EffectExecutor exec(state, events, card_db);
    ChainManager cm(state, events, card_db);
    TriggerManager tm(state, events, card_db, cm, card_registry);
    tm.setEffectExecutor(&exec);
    tm.subscribe();
    events.emit(ScoreEvent{P2, 0, ScoreMethod::Hold, 1});
    EXPECT_FALSE(onBoard(bomb)) << "opponent held the Bomb's battlefield";
    EXPECT_GE(chainSize(), 1) << "the Bomb's Deathknell went on the chain";
}

TEST_F(RadianceBombTest, Deploy_OwnHoldAndOtherBattlefieldKeepBomb) {
    auto mine = addGearAt(P1, kBomb, 0);
    auto elsewhere = addGearAt(P1, kBomb, 1);
    EffectExecutor exec(state, events, card_db);
    ChainManager cm(state, events, card_db);
    TriggerManager tm(state, events, card_db, cm, card_registry);
    tm.setEffectExecutor(&exec);
    tm.subscribe();
    events.emit(ScoreEvent{P1, 0, ScoreMethod::Hold, 1});
    EXPECT_TRUE(onBoard(mine)) << "your own hold doesn't kill your Deploy gear";
    events.emit(ScoreEvent{P2, 0, ScoreMethod::Hold, 1});
    EXPECT_FALSE(onBoard(mine));
    EXPECT_TRUE(onBoard(elsewhere)) << "only the held battlefield's gear dies";
}

TEST_F(RadianceBombTest, BombDeathknell_DealsTwoAndKillsOtherBombsThere) {
    auto a = addGearAt(P1, kBomb, 0);
    auto b = addGearAt(P1, kBomb, 0);
    auto far = addGearAt(P1, kBomb, 1);
    auto enemy = addUnit(P2, kInvalidId, /*might=*/5, /*at_bf=*/0);
    markDead(a);
    EffectExecutor exec(state, events, card_db);
    CardContext ctx{state, events, exec, P1, a};
    card_registry.get(kBomb)->onTrigger(ctx, {});
    EXPECT_EQ(state.getObject(enemy).damage_marked, 2);
    EXPECT_FALSE(onBoard(b)) << "other friendly Bomb at the same battlefield dies";
    EXPECT_TRUE(onBoard(far)) << "Bombs at other battlefields survive";
}

TEST_F(RadianceBombTest, ShortFused_AddsOneBonusDamage) {
    auto bomb = addGearAt(P1, kBomb, 0);
    auto enemy = addUnit(P2, kInvalidId, /*might=*/5, /*at_bf=*/0);
    addUnit(P1, kShortFused, 0, /*at_bf=*/1);
    markDead(bomb);
    EffectExecutor exec(state, events, card_db);
    CardContext ctx{state, events, exec, P1, bomb, TriggerType::WhenIDie, &card_registry};
    card_registry.get(kBomb)->onTrigger(ctx, {});
    EXPECT_EQ(state.getObject(enemy).damage_marked, 3);
}

TEST_F(RadianceBombTest, Explosimian_HoldPlaysThreeBombsToOneBattlefield) {
    for (auto& bf : state.battlefields) bf.card_object_id = state.createObject();
    auto monkey = addUnit(P1, kExplosimian, 0, /*at_bf=*/0);
    EffectExecutor exec(state, events, card_db);
    CardContext ctx{state, events, exec, P1, monkey, TriggerType::WhenIHold, &card_registry};
    card_registry.get(kExplosimian)->onTrigger(ctx, {});
    int bombs_at_0 = 0;
    for (const auto& [id, obj] : state.objects)
        if (obj.card_def_id == kBomb && obj.location.has_value() &&
            *obj.location == LocationId{BattlefieldLocation{0}})
            ++bombs_at_0;
    EXPECT_EQ(bombs_at_0, 3);
}

}  // namespace riftbound::test
