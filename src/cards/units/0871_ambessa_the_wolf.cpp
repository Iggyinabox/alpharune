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

class AmbessaTheWolf : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "[Empower] [3][O]"
    EmpowerSpec empowerSpec() const override { return empowerCost(3, 1, Domain::Body); }
    // "[Empowered][>] I have +3 [M] and can't be dealt damage unless I'm in
    // combat." In combat = holding an attacker/defender designation; the
    // immunity flag is re-asserted every aura pass (checked at the top of
    // EffectExecutor::dealDamage).
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) {
                auraMight(self, sid, 3);
                if (self.combat_designation == CombatDesignation::None)
                    self.immune_to_damage = true;
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 871;
        d.def_id = R"RB(ven-084-166)RB";
        d.name = R"RB(Ambessa, The Wolf)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-084/166)RB";
        d.collector_number = 84;
        d.artist = R"RB(Pandart Studio)RB";
        d.card_type = CardType::Unit;
        d.super_type = SuperType::Champion;
        d.domains = {Domain::Body};
        d.tags = {R"RB(Noxus)RB", R"RB(Ambessa)RB"};
        d.energy_cost = 4;
        d.might = 4;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [3][O] ([3][O]: Empower me. Use only if not Empowered.)
[Empowered][>] I have +3 [M] and can't be dealt damage unless I'm in combat.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/3907dc56d3a639e62f5222eaa0fd589c7a582da6-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_871(CardRegistry& r) {
    r.registerCard(871, std::make_unique<AmbessaTheWolf>());
}

}  // namespace riftbound
