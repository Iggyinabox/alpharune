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

class Recruit : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 954;
        d.def_id = R"RB(ven-t04)RB";
        d.name = R"RB(Recruit)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-T04)RB";
        d.collector_number = 4;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.super_type = SuperType::Token;
        d.tags = {R"RB(Recruit)RB"};
        d.might = 1;
        d.rarity = Rarity::Common;
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/08b9ccf225fc2ae9eb9b5668e5361b09789210ee-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_954(CardRegistry& r) {
    r.registerCard(954, std::make_unique<Recruit>());
}

}  // namespace riftbound
