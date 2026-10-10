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

class SandsweptTomb : public BattlefieldCard {
public:
    const CardDef& def() const override { return def_; }
    // "Each spell that chooses one or more units here that are friendly to it
    // costs [A] less."
    // The engine applies the published discount when the spell is played
    // with a friendly unit here among its targets.
    // ENGINE-GAP: spells that choose their targets as they resolve are not
    // known at payment time, so they do not get the discount.
    void applyPassiveAura(GameState& state, PlayerId /*controller*/) const override {
        for (auto& bf : state.battlefields) {
            if (!state.objectExists(bf.card_object_id)) continue;
            if (state.getObject(bf.card_object_id).card_def_id != cardDefId()) continue;
            bf.friendly_spell_discount_power = 1;
        }
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 951;
        d.def_id = R"RB(ven-164-166)RB";
        d.name = R"RB(Sandswept Tomb)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-164/166)RB";
        d.collector_number = 164;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Battlefield;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(Each spell that chooses one or more units here that are friendly to it costs [A] less.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/d77c607878d6815e02796f8bfa8a5696ad5f1a7d-1039x744.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_951(CardRegistry& r) {
    r.registerCard(951, std::make_unique<SandsweptTomb>());
}

}  // namespace riftbound
