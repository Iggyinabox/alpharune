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

class DragonRoost : public BattlefieldCard {
public:
    const CardDef& def() const override { return def_; }
    // "Any player may pay [A][A] as an additional cost to play a Dragon. If they
    // do, they play it to this battlefield."
    // ENGINE-GAP: optional additional costs are declared by the card being
    // played (Card::optionalAdditionalCost), not by a battlefield, and can't
    // grant a play location. Needs a battlefield-sourced additional-cost offer
    // in the play-unit generator for units tagged "Dragon" that adds this
    // battlefield as a destination when [A][A] is paid.
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 944;
        d.def_id = R"RB(ven-157-166)RB";
        d.name = R"RB(Dragon Roost)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-157/166)RB";
        d.collector_number = 157;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Battlefield;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(Any player may pay [A][A] as an additional cost to play a Dragon. If they do, they play it to this battlefield.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/94558ee0292d92126829036d32495cbf702e5391-1039x744.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_944(CardRegistry& r) {
    r.registerCard(944, std::make_unique<DragonRoost>());
}

}  // namespace riftbound
