#include "cards/card.h"
#include "cards/card_helpers.h"
#include "cards/card_registry.h"
#include "core/game_state.h"
#include "core/events.h"
#include "engine/effect_executor.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace riftbound {
namespace {

class MasterOfShadows : public LegendCard {
public:
    const CardDef& def() const override { return def_; }
    // "When you banish a card you own, empower me."
    TriggerType triggerType() const override { return TriggerType::WhenYouBanishACard; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.empower(ctx.source);
    }
    // "[Action][>] Disempower me, [E]: Discard 1, then draw 1."
    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        ActivationCost c;
        c.exhaust = true;
        c.disempower_self = true;
        return c;
    }
    bool isActionAbility() const override { return true; }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        discardOneThenAct(ctx, "Master of Shadows: discard 1",
            [](CardContext& c, GameObjectId) { c.executor.drawCards(c.controller, 1); });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 930;
        d.def_id = R"RB(ven-143-166)RB";
        d.name = R"RB(Master of Shadows)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-143/166)RB";
        d.collector_number = 143;
        d.artist = R"RB(Envar Studio)RB";
        d.card_type = CardType::Legend;
        d.domains = {Domain::Fury, Domain::Chaos};
        d.tags = {R"RB(Zed)RB"};
        d.rarity = Rarity::Rare;
        d.ability_text = R"RB(When you banish a card you own, empower me. (I become Empowered if I'm not already.)
[Action][>] Disempower me, [E]: Discard 1, then draw 1.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/aa316eb1b01c8d036eaba84b03909027fe69f6e2-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_930(CardRegistry& r) {
    r.registerCard(930, std::make_unique<MasterOfShadows>());
}

}  // namespace riftbound
