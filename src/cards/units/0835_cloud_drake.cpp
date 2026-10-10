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

class CloudDrake : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "When you play me, draw 1."
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayMe; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.drawCards(ctx.controller, 1);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 835;
        d.def_id = R"RB(ven-048-166)RB";
        d.name = R"RB(Cloud Drake)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-048/166)RB";
        d.collector_number = 48;
        d.artist = R"RB(Wild Blue Studios)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Mind};
        d.tags = {R"RB(Dragon)RB", R"RB(Demacia)RB"};
        d.energy_cost = 6;
        d.might = 5;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(When you play me, draw 1.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/cb0dbff1058e8c59fa041dd242aa9b43d92eae14-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_835(CardRegistry& r) {
    r.registerCard(835, std::make_unique<CloudDrake>());
}

}  // namespace riftbound
