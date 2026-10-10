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
    // The battlefield publishes the offer; the play generator adds this
    // battlefield as a destination for a Dragon that can't otherwise be played
    // here, and the engine charges the [A][A] on top of the unit's own cost.
    void applyPassiveAura(GameState& state, PlayerId /*controller*/) const override {
        for (auto& bf : state.battlefields) {
            if (!state.objectExists(bf.card_object_id)) continue;
            if (state.getObject(bf.card_object_id).card_def_id != cardDefId()) continue;
            bf.paid_play_power = 2;
            bf.paid_play_tag = "Dragon";
        }
    }
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
