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

class ShadowbladeLurker : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "I cost [2] less for each card with my name in your trash."
    int selfCostReduction(const GameState& state, PlayerId player) const override {
        return std::min(2 * namedInTrash(state, player, def().name), def().energy_cost);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 883;
        d.def_id = R"RB(ven-096-166)RB";
        d.name = R"RB(Shadowblade Lurker)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-096/166)RB";
        d.collector_number = 96;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Chaos};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 5;
        d.might = 5;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(I cost [2] less for each card with my name in your trash.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/f42e9286c968db8cd37fb2911617b8c78de7d516-744x1040.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_883(CardRegistry& r) {
    r.registerCard(883, std::make_unique<ShadowbladeLurker>());
}

}  // namespace riftbound
