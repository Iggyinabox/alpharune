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

// Runeblade Anvil (rad-012-167), Fury gear, 1 energy.
// "When you play this or when this leaves the board, banish the top card of
//  your Main Deck. Until your next Ending Phase, you may play that card.
//  [2][R], [E]: Kill this."
// APPROXIMATION: the engine has no play-from-banishment-until-Ending-Phase
// grant, so each banish-and-may-play is modeled as "draw 1". That overrates
// the card slightly (the drawn card does not expire). "Leaves the board" is
// modeled through its death (the gear Deathknell path); bounces are not seen.
class RunebladeAnvil : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    std::vector<TriggerType> triggerTypes() const override {
        return {TriggerType::WhenYouPlayThis, TriggerType::WhenIDie};
    }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        ctx.executor.drawCards(ctx.controller, 1);
    }
    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        return ActivationCost{.exhaust = true, .energy = 2, .power = 1,
                              .power_domain = Domain::Fury};
    }
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 0};
    }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>&) override {
        if (ctx.state.objectExists(ctx.source) &&
            ctx.state.getObject(ctx.source).location.has_value())
            ctx.executor.killObject(ctx.source);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 965;
        d.def_id = R"RB(rad-012-167)RB";
        d.name = R"RB(Runeblade Anvil)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-012/167)RB";
        d.collector_number = 12;
        d.card_type = CardType::Gear;
        d.domains = {Domain::Fury};
        d.energy_cost = 1;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(When you play this or when this leaves the board, banish the top card of your Main Deck. Until your next Ending Phase, you may play that card. (You still pay its costs.)
[2][R], [E]: Kill this.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_965(CardRegistry& r) {
    r.registerCard(965, std::make_unique<RunebladeAnvil>());
}

}  // namespace riftbound
