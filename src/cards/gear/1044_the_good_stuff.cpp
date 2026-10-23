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

// The Good Stuff (rad-106-167), Chaos gear, 3 energy + 1 Chaos.
// "[Deploy] When you play this, discard 1, then draw 3.
//  When you hold here, return this to its owner's hand."
// "When you hold here" for a gear = its controller holds its battlefield
// (fired as WhenIHold for gear by TriggerManager::fireHolderGearHoldTriggers).
class TheGoodStuff : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    bool hasDeploy() const override { return true; }
    std::vector<TriggerType> triggerTypes() const override {
        return {TriggerType::WhenYouPlayThis, TriggerType::WhenIHold};
    }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        if (ctx.firing_trigger == TriggerType::WhenIHold) {
            if (!ctx.state.objectExists(ctx.source)) return;
            if (!ctx.state.getObject(ctx.source).location.has_value()) return;
            ctx.executor.bounceToHand(ctx.source);
            return;
        }
        discardOneThenAct(ctx, "The Good Stuff: discard 1",
                          [](CardContext& c, GameObjectId) {
                              c.executor.drawCards(c.controller, 3);
                          });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 1044;
        d.def_id = R"RB(rad-106-167)RB";
        d.name = R"RB(The Good Stuff)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-106/167)RB";
        d.collector_number = 106;
        d.card_type = CardType::Gear;
        d.domains = {Domain::Chaos};
        d.energy_cost = 3;
        d.power_cost = 1;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB([Deploy] (Play this only to a battlefield. When an opponent holds here, kill this.)
When you play this, discard 1, then draw 3.
When you hold here, return this to its owner's hand.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_1044(CardRegistry& r) {
    r.registerCard(1044, std::make_unique<TheGoodStuff>());
}

}  // namespace riftbound
