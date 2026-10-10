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

class BaccaiSandspinner : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Empower] [5]. This ability costs [3] less if you control 4 or fewer runes."
    EmpowerSpec empowerSpec() const override { return empowerCost(5); }
    int activationCostReduction(const GameState& state, PlayerId controller,
                                int ability_index) const override {
        if (ability_index != empowerAbilityIndex()) return 0;
        return runesControlled(state, controller) <= 4 ? 3 : 0;
    }
    // "[Empowered][>] I have [Deflect] and [Assault 2]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) {
                auraKeyword(self, sid, Keyword::Deflect, 1);
                auraKeyword(self, sid, Keyword::Assault, 2);
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 788;
        d.def_id = R"RB(ven-001-166)RB";
        d.name = R"RB(Baccai Sandspinner)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-001/166)RB";
        d.collector_number = 1;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Shurima)RB"};
        d.energy_cost = 6;
        d.might = 6;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [5]. This ability costs [3] less if you control 4 or fewer runes. (Pay the cost: Empower me. Use only if not Empowered.)
[Empowered][>] I have [Deflect] and [Assault 2]. (Opponents must pay [A] to choose me with a spell or ability. +2 [M] while I'm an attacker.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/d8abab7f0df7c8ec9fff8c8dfa71c732e1ce2e6a-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_788(CardRegistry& r) {
    r.registerCard(788, std::make_unique<BaccaiSandspinner>());
}

}  // namespace riftbound
