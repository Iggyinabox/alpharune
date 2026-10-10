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

class BrutalHunter : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "[Empower] [3]"
    EmpowerSpec empowerSpec() const override { return empowerCost(3); }
    // "[Empowered][>] I have +2 [M] and [Ganking]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) {
                auraMight(self, sid, 2);
                auraKeyword(self, sid, Keyword::Ganking);
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 857;
        d.def_id = R"RB(ven-070-166)RB";
        d.name = R"RB(Brutal Hunter)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-070/166)RB";
        d.collector_number = 70;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Body};
        d.tags = {R"RB(Noxus)RB"};
        d.energy_cost = 3;
        d.power_cost = 1;
        d.might = 4;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [3] ([3]: Empower me. Use only if not Empowered.)
[Empowered][>] I have +2 [M] and [Ganking]. (I can move from battlefield to battlefield.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/54122304a3bb1f989faf124cb74fd7d41b029681-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_857(CardRegistry& r) {
    r.registerCard(857, std::make_unique<BrutalHunter>());
}

}  // namespace riftbound
