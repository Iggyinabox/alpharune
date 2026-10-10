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

class ShadowAssassin : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "I enter ready if you have a card with my name in your trash."
    bool entersReadyOnPlay(const GameState& state, PlayerId controller) const override {
        return namedInTrash(state, controller, def().name) > 0;
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 800;
        d.def_id = R"RB(ven-013-166)RB";
        d.name = R"RB(Shadow Assassin)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-013/166)RB";
        d.collector_number = 13;
        d.artist = R"RB(JiHun Lee)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 5;
        d.might = 5;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(I enter ready if you have a card with my name in your trash.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/d5c1aed33b6aa109ff2baea8b3e0c146af750dcc-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_800(CardRegistry& r) {
    r.registerCard(800, std::make_unique<ShadowAssassin>());
}

}  // namespace riftbound
