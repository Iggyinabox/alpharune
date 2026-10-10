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

class AppliedResearchers : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "[Empower] [3]"
    EmpowerSpec empowerSpec() const override { return empowerCost(3); }
    // "[Empowered][>] Your spells cost [1][A] less, to a minimum of [1]."
    // Asserts the continuous per-player discount; each Empowered copy stacks.
    // The energy half is applied by GameEngine::canAfford / payCardCost
    // (spell_discount_energy, floored at [1]); the [A] half is
    // PlayerState::spell_discount_power, subtracted from a spell's Power cost
    // by the same path.
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [&](GameObjectId, GameObject&) {
                auto& ps = state.player(controller);
                ps.spell_discount_energy += 1;
                ps.spell_discount_power += 1;
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 842;
        d.def_id = R"RB(ven-055-166)RB";
        d.name = R"RB(Applied Researchers)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-055/166)RB";
        d.collector_number = 55;
        d.artist = R"RB(Polar Engine Studio)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Mind};
        d.tags = {R"RB(Shurima)RB"};
        d.energy_cost = 4;
        d.might = 4;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [3] ([3]: Empower me. Use only if not Empowered.)
[Empowered][>] Your spells cost [1][A] less, to a minimum of [1].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/67a5f67bad4d4201b14760206a34c838d7a1e4dc-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_842(CardRegistry& r) {
    r.registerCard(842, std::make_unique<AppliedResearchers>());
}

}  // namespace riftbound
