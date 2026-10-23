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

// Bomb (rad-t02), token gear.
// "[Deploy] [Deathknell] — Deal 2 to an enemy unit here and kill all other
//  Bombs you control here."
// The chain reaction comes from each killed Bomb firing its own Deathknell.
// Detonation Amplifier's extra fires are counted by the TriggerManager.
class Bomb_1102 : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    bool hasDeploy() const override { return true; }
    TriggerType triggerType() const override { return TriggerType::WhenIDie; }

    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        const auto& self = ctx.state.getObject(ctx.source);
        const auto where = self.location.has_value() ? self.location : self.last_location;
        if (!where.has_value() || !std::holds_alternative<BattlefieldLocation>(*where)) return;
        const LocationId here = *where;

        std::vector<GameObjectId> enemies;
        for (const auto& [id, obj] : ctx.state.objects) {
            if (!obj.isUnit() || obj.controller == ctx.controller) continue;
            if (obj.location.has_value() && *obj.location == here) enemies.push_back(id);
        }
        auto target = pickTarget(ctx, "Bomb: enemy unit to deal 2", enemies);
        if (target == kInvalidId && pickSuspended(ctx)) return;

        if (target != kInvalidId && ctx.state.objectExists(target)) {
            const int dmg = 2 + friendlyGearBonusDamage(ctx);
            ctx.executor.dealDamage(target, dmg, ctx.source);
            if (ctx.state.objectExists(target) &&
                ctx.state.getObject(target).hasLethalDamage())
                ctx.executor.killObject(target);
        }

        // Kill all other Bombs you control here (each fires its Deathknell).
        std::vector<GameObjectId> others;
        for (const auto& [id, obj] : ctx.state.objects) {
            if (id == ctx.source || obj.controller != ctx.controller) continue;
            if (obj.card_def_id != kBombGearCardDefId) continue;
            if (obj.location.has_value() && *obj.location == here) others.push_back(id);
        }
        for (auto id : others)
            if (ctx.state.objectExists(id)) ctx.executor.killObject(id);
    }

private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 1102;
        d.def_id = R"RB(rad-t02)RB";
        d.name = R"RB(Bomb)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-T02)RB";
        d.card_type = CardType::Gear;
        d.super_type = SuperType::Token;
        d.tags = {R"RB(Bomb)RB"};
        d.keywords.set(Keyword::Deathknell);
        d.ability_text = R"RB([Deploy] (Play this only to a battlefield. When an opponent holds here, kill this.)
[Deathknell] — Deal 2 to an enemy unit here and kill all other Bombs you control here.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_1102(CardRegistry& r) {
    r.registerCard(1102, std::make_unique<Bomb_1102>());
}

}  // namespace riftbound
