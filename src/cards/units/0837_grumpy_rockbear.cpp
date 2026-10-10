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

class GrumpyRockbear : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "[Empower] [12]. This ability costs [1] less for each rune you control."
    EmpowerSpec empowerSpec() const override { return empowerCost(12); }
    int activationCostReduction(const GameState& state, PlayerId controller,
                                int ability_index) const override {
        if (ability_index != empowerAbilityIndex()) return 0;
        return runesControlled(state, controller);
    }
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
        d.id = 837;
        d.def_id = R"RB(ven-050-166)RB";
        d.name = R"RB(Grumpy Rockbear)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-050/166)RB";
        d.collector_number = 50;
        d.artist = R"RB(Dao Trong Le)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Mind};
        d.tags = {R"RB(Shurima)RB"};
        d.energy_cost = 4;
        d.might = 4;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [12]. This ability costs [1] less for each rune you control. (Pay the cost: Empower me. Use only if not Empowered.)
[Empowered][>] I have [Deflect] and [Shield 3]. (Opponents must pay [A] to choose me with a spell or ability.+3 [M] while I'm a defender.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/2f7f3759c59746486a5f91b8452302fa59eddde6-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_837(CardRegistry& r) {
    r.registerCard(837, std::make_unique<GrumpyRockbear>());
}

}  // namespace riftbound
