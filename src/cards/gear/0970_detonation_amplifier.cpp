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

// Detonation Amplifier (rad-017-167), Fury gear, 4 energy + 1 Fury.
// "[Deploy] When you play this, you may kill another friendly unit or gear to
//  draw 1. Friendly [Deathknell] effects here trigger an additional time."
// The extra Deathknell fires are counted by TriggerManager::extraDeathknellFires.
class DetonationAmplifier : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    bool hasDeploy() const override { return true; }
    int extraDeathknellTriggersHere() const override { return 1; }
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayThis; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto fodder = [&]() {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if ((obj.isUnit() || obj.isGear()) && obj.controller == ctx.controller &&
                    obj.location.has_value() && id != ctx.source && !obj.attached_to.has_value())
                    out.push_back(id);
            return out;
        };
        int go = confirmOptional(ctx, "Detonation Amplifier: kill a friendly unit or gear to draw 1?",
                                 [&]() { return !fodder().empty(); });
        if (go <= 0) return;
        auto sac = pickTarget(ctx, "Detonation Amplifier: friendly unit or gear to kill", fodder());
        if (sac == kInvalidId || !ctx.state.objectExists(sac)) return;
        ctx.executor.killObject(sac);
        ctx.executor.drawCards(ctx.controller, 1);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 970;
        d.def_id = R"RB(rad-017-167)RB";
        d.name = R"RB(Detonation Amplifier)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-017/167)RB";
        d.collector_number = 17;
        d.card_type = CardType::Gear;
        d.domains = {Domain::Fury};
        d.energy_cost = 4;
        d.power_cost = 1;
        d.rarity = Rarity::Rare;
        d.ability_text = R"RB([Deploy] (Play this only to a battlefield. When an opponent holds here, kill this.)
When you play this, you may kill another friendly unit or gear to draw 1.
Friendly [Deathknell] effects here trigger an additional time.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_970(CardRegistry& r) {
    r.registerCard(970, std::make_unique<DetonationAmplifier>());
}

}  // namespace riftbound
