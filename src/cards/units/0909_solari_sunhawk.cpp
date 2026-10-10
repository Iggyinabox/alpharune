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

class SolariSunhawk : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "[Empower] [2]"
    EmpowerSpec empowerSpec() const override { return empowerCost(2); }
    // "[Empowered][>] I have +1 [M] and [Deflect 2]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller, [](GameObjectId sid, GameObject& self) {
            auraMight(self, sid, 1);
            auraKeyword(self, sid, Keyword::Deflect, 2);
        });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 909;
        d.def_id = R"RB(ven-122-166)RB";
        d.name = R"RB(Solari Sunhawk)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-122/166)RB";
        d.collector_number = 122;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Bird)RB", R"RB(Mount Targon)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [2] ([2]: Empower me. Use only if not Empowered.)
[Empowered][>] I have +1 [M] and [Deflect 2]. (Opponents must pay [A][A] to choose me with a spell or ability.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/1aaf389ca46bd9122233aadcfd23fee8f9cdfdc9-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_909(CardRegistry& r) {
    r.registerCard(909, std::make_unique<SolariSunhawk>());
}

}  // namespace riftbound
