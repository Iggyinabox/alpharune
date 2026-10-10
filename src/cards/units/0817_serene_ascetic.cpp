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

class SereneAscetic : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Empower] [3]"
    EmpowerSpec empowerSpec() const override { return empowerCost(3); }
    // "[Empowered][>] I have [Deflect] and [Shield 3]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) {
                auraKeyword(self, sid, Keyword::Deflect, 1);
                auraKeyword(self, sid, Keyword::Shield, 3);
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 817;
        d.def_id = R"RB(ven-030-166)RB";
        d.name = R"RB(Serene Ascetic)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-030/166)RB";
        d.collector_number = 30;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Calm};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [3] ([3]: Empower me. Use only if not Empowered.)
[Empowered][>] I have [Deflect] and [Shield 3]. (Opponents must pay [A] to choose me with a spell or ability.+3 [M] while I'm a defender.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/7acc2f9fcdec5d2649e4a50dc3c96aeb31b87916-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_817(CardRegistry& r) {
    r.registerCard(817, std::make_unique<SereneAscetic>());
}

}  // namespace riftbound
