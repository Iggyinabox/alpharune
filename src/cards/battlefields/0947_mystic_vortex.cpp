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

class MysticVortex : public BattlefieldCard {
public:
    const CardDef& def() const override { return def_; }
    // "During showdowns here, cards with [Reaction] cost [A] more to play.
    // (Hidden cards have [Reaction].)"
    // ENGINE-GAP: the tax is published on the battlefield, but the play-cost
    // path (canAfford / beginCostPayment) does not read
    // BattlefieldState::reaction_tax_power yet. Needs: while a showdown is in
    // progress at a battlefield with this flag, add that much any-domain power
    // to the cost of cards with [Reaction] (including [Ambush] units played
    // there and cards played from face down).
    void applyPassiveAura(GameState& state, PlayerId /*controller*/) const override {
        for (auto& bf : state.battlefields) {
            if (!state.objectExists(bf.card_object_id)) continue;
            if (state.getObject(bf.card_object_id).card_def_id != cardDefId()) continue;
            if (bf.showdown_in_progress || bf.combat_in_progress) bf.reaction_tax_power = 1;
        }
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 947;
        d.def_id = R"RB(ven-160-166)RB";
        d.name = R"RB(Mystic Vortex)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-160/166)RB";
        d.collector_number = 160;
        d.artist = R"RB(Caravan Studio)RB";
        d.card_type = CardType::Battlefield;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(During showdowns here, cards with [Reaction] cost [A] more to play. (Hidden cards have [Reaction].))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/55f6fec944c489010464cb554c4661d94518fa75-1039x744.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_947(CardRegistry& r) {
    r.registerCard(947, std::make_unique<MysticVortex>());
}

}  // namespace riftbound
