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

class PunchingPoro : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Empower] — Discard 1"
    EmpowerSpec empowerSpec() const override {
        EmpowerSpec s;
        s.valid = true;
        s.cost.discard = true;
        s.cost.discard_count = 1;
        return s;
    }
    // "[Empowered][>] I have +1 [M]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) { auraMight(self, sid, 1); });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 794;
        d.def_id = R"RB(ven-007-166)RB";
        d.name = R"RB(Punching Poro)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-007/166)RB";
        d.collector_number = 7;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Poro)RB", R"RB(Ionia)RB"};
        d.energy_cost = 2;
        d.might = 2;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] — Discard 1 (Pay the cost: Empower me. Use only if not Empowered.)
[Empowered][>] I have +1 [M].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/03b9a8e704e0f09b686d89d4c2d79f100f212b0f-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_794(CardRegistry& r) {
    r.registerCard(794, std::make_unique<PunchingPoro>());
}

}  // namespace riftbound
