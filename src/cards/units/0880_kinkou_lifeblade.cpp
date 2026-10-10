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

class KinkouLifeblade : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "[Empower] [2]"
    EmpowerSpec empowerSpec() const override { return empowerCost(2); }
    // "[Empowered][>] I have +1 [M] and [Ganking]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) {
                auraMight(self, sid, 1);
                auraKeyword(self, sid, Keyword::Ganking);
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 880;
        d.def_id = R"RB(ven-093-166)RB";
        d.name = R"RB(Kinkou Lifeblade)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-093/166)RB";
        d.collector_number = 93;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Chaos};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 4;
        d.might = 4;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [2] ([2]: Empower me. Use only if not Empowered.)
[Empowered][>] I have +1 [M] and [Ganking]. (I can move from battlefield to battlefield.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/18e925b4291231a2045e60d062bd174950316c1d-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_880(CardRegistry& r) {
    r.registerCard(880, std::make_unique<KinkouLifeblade>());
}

}  // namespace riftbound
