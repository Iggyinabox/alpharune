#include "cards/card.h"
#include "cards/card_registry.h"
#include "core/game_state.h"
#include "core/events.h"
#include "engine/effect_executor.h"
#include <algorithm>
#include <memory>
#include <vector>
#include "cards/card_helpers.h"

namespace riftbound {
namespace {

// Scorched Earth (rad-006-167), Fury spell, 4 energy + 1 Fury.
// "[Action] Deal 5 damage split among units at a single battlefield."
// The battlefield is an agent choice. APPROXIMATION: the split is assigned by
// a fixed policy rather than per point: enemy units only, cheapest kills first
// (fewest points to lethal), any remainder onto the largest enemy left.
class ScorchedEarth : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto enemies_at = [&](const LocationId& loc) {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if (obj.isUnit() && obj.controller != ctx.controller &&
                    obj.location.has_value() && *obj.location == loc)
                    out.push_back(id);
            return out;
        };
        std::vector<GameObjectId> bfs;
        for (const auto& bf : ctx.state.battlefields)
            if (ctx.state.objectExists(bf.card_object_id) &&
                !enemies_at(LocationId{BattlefieldLocation{bf.id}}).empty())
                bfs.push_back(bf.card_object_id);
        auto picked = pickTarget(ctx, "Scorched Earth: battlefield", bfs);
        if (picked == kInvalidId) return;
        auto bf = battlefieldIdOfCardObject(ctx.state, picked);
        if (!bf) return;

        auto enemies = enemies_at(LocationId{BattlefieldLocation{*bf}});
        auto to_lethal = [&](GameObjectId id) {
            const auto& o = ctx.state.getObject(id);
            return std::max(1, o.current_might - o.damage_marked);
        };
        std::sort(enemies.begin(), enemies.end(), [&](GameObjectId a, GameObjectId b) {
            return to_lethal(a) < to_lethal(b);
        });
        int budget = 5;
        std::vector<std::pair<GameObjectId, int>> plan;
        for (auto id : enemies) {
            int need = to_lethal(id);
            if (need > budget) continue;
            plan.push_back({id, need});
            budget -= need;
        }
        if (budget > 0) {
            GameObjectId biggest = kInvalidId;
            int best = -1;
            for (auto id : enemies) {
                bool planned = std::any_of(plan.begin(), plan.end(),
                                           [&](const auto& p) { return p.first == id; });
                if (planned) continue;
                if (ctx.state.getObject(id).current_might > best) {
                    best = ctx.state.getObject(id).current_might;
                    biggest = id;
                }
            }
            if (biggest != kInvalidId) plan.push_back({biggest, budget});
            else if (!plan.empty()) plan.front().second += budget;
        }
        // Deal all damage first, then kill (AoE rule in CLAUDE.md).
        for (const auto& [id, amount] : plan)
            if (ctx.state.objectExists(id)) ctx.executor.dealDamage(id, amount, ctx.source);
        for (const auto& [id, amount] : plan)
            if (ctx.state.objectExists(id) && ctx.state.getObject(id).hasLethalDamage())
                ctx.executor.killObject(id);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 961;
        d.def_id = R"RB(rad-006-167)RB";
        d.name = R"RB(Scorched Earth)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-006/167)RB";
        d.collector_number = 6;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Fury};
        d.energy_cost = 4;
        d.power_cost = 1;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.) Deal 5 damage split among units at a single battlefield.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_961(CardRegistry& r) {
    r.registerCard(961, std::make_unique<ScorchedEarth>());
}

}  // namespace riftbound
