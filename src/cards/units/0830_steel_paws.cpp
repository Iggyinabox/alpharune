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

class SteelPaws : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // [Deflect] is data (def_.deflect_value).
    // "[Empower] [7]"
    EmpowerSpec empowerSpec() const override { return empowerCost(7); }
    // "[Empowered][>] I have +7 [M]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) { auraMight(self, sid, 7); });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 830;
        d.def_id = R"RB(ven-043-166)RB";
        d.name = R"RB(Steel Paws)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-043/166)RB";
        d.collector_number = 43;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Calm};
        d.tags = {R"RB(Cat)RB", R"RB(Piltover)RB"};
        d.energy_cost = 1;
        d.might = 0;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Deflect);
        d.deflect_value = 1;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Deflect] (Opponents must pay [A] to choose me with a spell or ability.)
[Empower] [7] ([7]: Empower me. Use only if not Empowered.)
[Empowered][>] I have +7 [M].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/79d9b6583ed8e7d5a1486f1c15b66b19fb7f69a5-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_830(CardRegistry& r) {
    r.registerCard(830, std::make_unique<SteelPaws>());
}

}  // namespace riftbound
