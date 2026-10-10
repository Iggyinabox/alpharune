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

class HelmOfSuppression : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    // [vendetta-E]
    // "[Empower] [4][G]"
    EmpowerSpec empowerSpec() const override { return empowerCost(4, 1, Domain::Calm); }
    // "Opponents' spells cost [1] more. If this is [Empowered], they cost
    //  [1][A] more instead." The tax is recorded on the TAXED player.
    // The Energy half is applied by the play-cost path (GameEngine canAfford /
    // payCardCost read PlayerState::spell_tax_energy).
    // ENGINE-GAP: the [A] half of the Empowered tax is not applied.
    // PlayerState::spell_tax_power is reset every aura pass but nothing reads
    // it: the same spell cost path must add spell_tax_power any-domain Power
    // to the spell's power cost.
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachSelfOnce(*this, state, controller, [&](GameObjectId, GameObject& self) {
            auto& opp = state.player(opponent(controller));
            opp.spell_tax_energy += 1;
            if (self.isEmpowered()) opp.spell_tax_power += 1;
        });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 832;
        d.def_id = R"RB(ven-045-166)RB";
        d.name = R"RB(Helm of Suppression)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-045/166)RB";
        d.collector_number = 45;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Gear;
        d.domains = {Domain::Calm};
        d.energy_cost = 4;
        d.power_cost = 1;
        d.rarity = Rarity::Epic;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [4][G] ([4][G]: Empower this. Use only if not Empowered.)
Opponents' spells cost [1] more. If this is [Empowered], they cost [1][A] more instead.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/ecfe67bbdc04f5c16a845ccc7a1b8d534195ca70-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_832(CardRegistry& r) {
    r.registerCard(832, std::make_unique<HelmOfSuppression>());
}

}  // namespace riftbound
