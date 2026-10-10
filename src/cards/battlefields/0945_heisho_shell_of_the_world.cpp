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

class HeishoShellOfTheWorld : public BattlefieldCard {
public:
    const CardDef& def() const override { return def_; }
    // "Players ignore [Deflect] while paying for spells and abilities choosing
    // something here."
    // The flag is asserted for the cost path. Note the engine does not yet
    // charge [Deflect] costs at all (deflect_value is tracked but unpaid), so
    // this has no observable effect until Deflect payment exists.
    void applyPassiveAura(GameState& state, PlayerId /*controller*/) const override {
        for (auto& bf : state.battlefields) {
            if (!state.objectExists(bf.card_object_id)) continue;
            if (state.getObject(bf.card_object_id).card_def_id != cardDefId()) continue;
            bf.ignore_deflect_here = true;
        }
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 945;
        d.def_id = R"RB(ven-158-166)RB";
        d.name = R"RB(Heisho, Shell of the World)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-158/166)RB";
        d.collector_number = 158;
        d.artist = R"RB(Envar Studio)RB";
        d.card_type = CardType::Battlefield;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(Players ignore [Deflect] while paying for spells and abilities choosing something here.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/075d4b78c065a96845ac978c86af3a3349cb5501-1039x744.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_945(CardRegistry& r) {
    r.registerCard(945, std::make_unique<HeishoShellOfTheWorld>());
}

}  // namespace riftbound
