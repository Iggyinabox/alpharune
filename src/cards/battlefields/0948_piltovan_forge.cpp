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

class PiltovanForge : public BattlefieldCard {
public:
    const CardDef& def() const override { return def_; }
    // "While you control this battlefield, the first friendly gear activated
    // ability played each turn costs [1] less."
    // ENGINE-GAP: needs a per-player "gear abilities activated this turn"
    // counter and a controller-scoped discount read by the activation cost
    // path (Card::activationCostReductionFor sees only the source card).
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 948;
        d.def_id = R"RB(ven-161-166)RB";
        d.name = R"RB(Piltovan Forge)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-161/166)RB";
        d.collector_number = 161;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Battlefield;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(While you control this battlefield, the first friendly gear activated ability played each turn costs [1] less.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/0cc432b51e80d1734529ef323e601e62e202c2b1-1039x744.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_948(CardRegistry& r) {
    r.registerCard(948, std::make_unique<PiltovanForge>());
}

}  // namespace riftbound
