#include "cards/card.h"
#include "cards/card_registry.h"
#include "core/game_state.h"
#include "core/events.h"
#include "engine/effect_executor.h"
#include <memory>
#include <vector>
#include "cards/card_helpers.h"

namespace riftbound {
namespace {

// Hexplosive Minefield (rad-142-167), Ziggs signature spell, Fury + Chaos,
// 2 energy + 1 power.
// "[Action] Kill any number of friendly Bomb tokens. At end of turn, play that
//  many Bomb tokens to a single battlefield."
// APPROXIMATION of "any number": the agent picks one battlefield and every
// friendly Bomb there is killed (killing one there kills the rest anyway via
// its Deathknell). At end of turn a delayed ability asks for the battlefield
// for the replacements.
class HexplosiveMinefield : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    static constexpr const char* kPendingKey = "__minefield_pending_bombs";

    void onResolve(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto bombs_at = [&](const LocationId& loc) {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if (obj.card_def_id == kBombGearCardDefId && obj.controller == ctx.controller &&
                    obj.location.has_value() && *obj.location == loc)
                    out.push_back(id);
            return out;
        };
        std::vector<GameObjectId> bfs;
        for (const auto& bf : ctx.state.battlefields)
            if (ctx.state.objectExists(bf.card_object_id) &&
                !bombs_at(LocationId{BattlefieldLocation{bf.id}}).empty())
                bfs.push_back(bf.card_object_id);
        auto picked = pickTarget(ctx, "Hexplosive Minefield: detonate Bombs at", bfs);
        if (picked == kInvalidId) return;
        auto bf = battlefieldIdOfCardObject(ctx.state, picked);
        if (!bf) return;
        auto bombs = bombs_at(LocationId{BattlefieldLocation{*bf}});
        for (auto id : bombs)
            if (ctx.state.objectExists(id)) ctx.executor.killObject(id);
        if (bombs.empty() || !ctx.state.objectExists(ctx.source)) return;

        ctx.state.getObject(ctx.source).card_counters[kPendingKey] =
            static_cast<int>(bombs.size());
        DelayedAbility da;
        da.source = ctx.source;
        da.card_def_id = cardDefId();
        da.controller = ctx.controller;
        da.trigger = TriggerType::AtEndOfTurn;
        da.expires_on_turn = ctx.state.turn.turn_number;
        ctx.state.delayed_abilities.push_back(da);
    }

    // Delayed: "At end of turn, play that many Bomb tokens to a single battlefield."
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        auto& me = ctx.state.getObject(ctx.source);
        auto it = me.card_counters.find(kPendingKey);
        if (it == me.card_counters.end() || it->second <= 0) return;
        const int n = it->second;
        auto picked = pickTarget(ctx, "Hexplosive Minefield: battlefield for the new Bombs",
                                 battlefieldCardObjects(ctx.state));
        if (picked == kInvalidId) return;
        me.card_counters.erase(kPendingKey);
        if (auto bf = battlefieldIdOfCardObject(ctx.state, picked))
            for (int i = 0; i < n; ++i) playBombToken(ctx, *bf);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 1075;
        d.def_id = R"RB(rad-142-167)RB";
        d.name = R"RB(Hexplosive Minefield)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-142/167)RB";
        d.collector_number = 142;
        d.card_type = CardType::Spell;
        d.super_type = SuperType::Signature;
        d.domains = {Domain::Fury, Domain::Chaos};
        d.tags = {R"RB(Ziggs)RB"};
        d.energy_cost = 2;
        d.power_cost = 1;
        d.rarity = Rarity::Epic;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.)
Kill any number of friendly Bomb tokens. At end of turn, play that many Bomb tokens to a single battlefield.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_1075(CardRegistry& r) {
    r.registerCard(1075, std::make_unique<HexplosiveMinefield>());
}

}  // namespace riftbound
