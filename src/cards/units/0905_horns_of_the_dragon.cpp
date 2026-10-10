#include "cards/card.h"
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

class HornsOfTheDragon : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 905;
        d.def_id = R"RB(ven-118-166)RB";
        d.name = R"RB(Horns of the Dragon)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-118/166)RB";
        d.collector_number = 118;
        d.artist = R"RB(JiHun Lee)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 6;
        d.might = 6;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Tank);
        d.ability_text = R"RB([Tank] (I must be assigned combat damage first.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/a7b3cb9f4175e71bc8fb60e69c55a0ea3f26fe8f-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_905(CardRegistry& r) {
    r.registerCard(905, std::make_unique<HornsOfTheDragon>());
}

}  // namespace riftbound
