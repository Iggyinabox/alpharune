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

class LegionMarauder : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "[Empower] — [1] or [O]". The engine's ActivationCost models one cost, so
    // the [1] (energy) option is offered; the alternative [O] payment is not.
    EmpowerSpec empowerSpec() const override { return empowerCost(1); }
    // "[Empowered][>] I have +1 [M]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) { auraMight(self, sid, 1); });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 861;
        d.def_id = R"RB(ven-074-166)RB";
        d.name = R"RB(Legion Marauder)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-074/166)RB";
        d.collector_number = 74;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Body};
        d.tags = {R"RB(Trifarian)RB", R"RB(Noxus)RB"};
        d.energy_cost = 2;
        d.might = 2;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] — [1] or [O] (Pay either cost: Empower me. Use only if not Empowered.)
[Empowered][>] I have +1 [M].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/15ec9e813e58c9ba96f37e78f09faaaa184ecdad-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_861(CardRegistry& r) {
    r.registerCard(861, std::make_unique<LegionMarauder>());
}

}  // namespace riftbound
