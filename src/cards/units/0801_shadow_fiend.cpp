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

class ShadowFiend : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Empower] [2][R]"
    EmpowerSpec empowerSpec() const override { return empowerCost(2, 1, Domain::Fury); }
    // "[Empowered][>] I have [Assault 3]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) {
                auraKeyword(self, sid, Keyword::Assault, 3);
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 801;
        d.def_id = R"RB(ven-014-166)RB";
        d.name = R"RB(Shadow Fiend)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-014/166)RB";
        d.collector_number = 14;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 2;
        d.might = 2;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [2][R] ([2][R]: Empower me. Use only if not Empowered.)
[Empowered][>] I have [Assault 3]. (+3 [M] while I'm an attacker.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/3bbeed15ac517c2e39c039e376553ce7e1c65f0c-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_801(CardRegistry& r) {
    r.registerCard(801, std::make_unique<ShadowFiend>());
}

}  // namespace riftbound
