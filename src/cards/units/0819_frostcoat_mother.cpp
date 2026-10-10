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

class FrostcoatMother : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Empower] [12]. This ability costs [1] less for each rune you control."
    EmpowerSpec empowerSpec() const override { return empowerCost(12); }
    int activationCostReduction(const GameState& state, PlayerId controller,
                                int ability_index) const override {
        if (ability_index != empowerAbilityIndex()) return 0;
        return runesControlled(state, controller);
    }
    // "[Empowered][>] I have +3 [M]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) { auraMight(self, sid, 3); });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 819;
        d.def_id = R"RB(ven-032-166)RB";
        d.name = R"RB(Frostcoat Mother)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-032/166)RB";
        d.collector_number = 32;
        d.artist = R"RB(Caravan Studio)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Calm};
        d.tags = {R"RB(Dog)RB", R"RB(Freljord)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [12]. This ability costs [1] less for each rune you control. (Pay the cost: Empower me. Use only if not Empowered.)
[Empowered][>] I have +3 [M].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/38e2b4f12346a1af420f875c0a5f922c56c4140f-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_819(CardRegistry& r) {
    r.registerCard(819, std::make_unique<FrostcoatMother>());
}

}  // namespace riftbound
