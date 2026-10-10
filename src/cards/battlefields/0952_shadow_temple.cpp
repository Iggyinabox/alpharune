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

class ShadowTemple : public BattlefieldCard {
public:
    const CardDef& def() const override { return def_; }
    // "When you hold here, [Burn 3]."
    TriggerType triggerType() const override { return TriggerType::WhenYouHoldHere; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.burn(ctx.controller, 3);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 952;
        d.def_id = R"RB(ven-165-166)RB";
        d.name = R"RB(Shadow Temple)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-165/166)RB";
        d.collector_number = 165;
        d.artist = R"RB(Grafit Studio)RB";
        d.card_type = CardType::Battlefield;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(When you hold here, [Burn 3]. (Put the top 3 cards of your Main Deck into your trash.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/635a3a0b724a2cfc11938ecbc1d8abddd89d3436-1039x744.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_952(CardRegistry& r) {
    r.registerCard(952, std::make_unique<ShadowTemple>());
}

}  // namespace riftbound
