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

class PlazaGuardian : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "I cost [1] less for each gear you control." ([Deflect] is engine-handled.)
    int selfCostReduction(const GameState& state, PlayerId player) const override {
        return std::min(gearControlled(state, player), def().energy_cost);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 851;
        d.def_id = R"RB(ven-064-166)RB";
        d.name = R"RB(Plaza Guardian)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-064/166)RB";
        d.collector_number = 64;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Mind};
        d.tags = {R"RB(Mech)RB", R"RB(Piltover)RB"};
        d.energy_cost = 10;
        d.might = 8;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Deflect);
        d.deflect_value = 1;
        d.ability_text = R"RB(I cost [1] less for each gear you control.
[Deflect] (Opponents must pay [A] to choose me with a spell or ability.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/3fc86ed4d23604dbb3c7de211eb6a9c36bd9176d-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_851(CardRegistry& r) {
    r.registerCard(851, std::make_unique<PlazaGuardian>());
}

}  // namespace riftbound
