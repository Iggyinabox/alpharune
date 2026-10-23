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

// Lost to the Sands (rad-013-167), Fury spell, 3 energy + 1 Fury.
// "[Action] Choose one — Deal 4 to a unit at a battlefield. / Kill a gear."
class LostToTheSands : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto units_at_bf = [&]() {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if (obj.isUnit() && obj.isAtBattlefield()) out.push_back(id);
            return out;
        };
        auto gear = [&]() {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if (obj.isGear() && !obj.isUnit() && obj.location.has_value()) out.push_back(id);
            return out;
        };
        uint32_t legal = 0;
        if (!units_at_bf().empty()) legal |= 1u;
        if (!gear().empty()) legal |= 2u;
        int mode = pickMode(ctx, "Lost to the Sands", 2,
                            {"Deal 4 to a unit at a battlefield", "Kill a gear"}, legal);
        if (mode < 0) return;   // suspended or nothing legal
        if (mode == 0) {
            auto t = pickTarget(ctx, "Lost to the Sands: unit to deal 4", units_at_bf());
            if (t == kInvalidId || !ctx.state.objectExists(t)) return;
            ctx.executor.dealDamage(t, 4, ctx.source);
            if (ctx.state.objectExists(t) && ctx.state.getObject(t).hasLethalDamage())
                ctx.executor.killObject(t);
        } else {
            auto t = pickTarget(ctx, "Lost to the Sands: gear to kill", gear());
            if (t == kInvalidId || !ctx.state.objectExists(t)) return;
            ctx.executor.killObject(t);
        }
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 966;
        d.def_id = R"RB(rad-013-167)RB";
        d.name = R"RB(Lost to the Sands)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-013/167)RB";
        d.collector_number = 13;
        d.card_type = CardType::Spell;
        d.domains = {Domain::Fury};
        d.energy_cost = 3;
        d.power_cost = 1;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.)
Choose one —
Deal 4 to a unit at a battlefield.
Kill a gear.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_966(CardRegistry& r) {
    r.registerCard(966, std::make_unique<LostToTheSands>());
}

}  // namespace riftbound
