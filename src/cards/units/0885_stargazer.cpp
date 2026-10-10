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

class Stargazer : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "Spells with [Flow] you play from your trash cost [2] less, to a minimum
    // of [1]." The engine applies PlayerState::flow_cost_reduction (floored at
    // [1]) when it builds the Flow play options; each Stargazer adds 2.
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachSelfOnce(*this, state, controller,
            [&](GameObjectId, GameObject& self) {
                if (!self.location.has_value()) return;
                state.player(controller).flow_cost_reduction += 2;
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 885;
        d.def_id = R"RB(ven-098-166)RB";
        d.name = R"RB(Stargazer)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-098/166)RB";
        d.collector_number = 98;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Chaos};
        d.tags = {R"RB(Mount Targon)RB"};
        d.energy_cost = 5;
        d.might = 4;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(Spells with [Flow] you play from your trash cost [2] less, to a minimum of [1].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/72ef8764cb3909940f0ca4bd8b31cdceed241af0-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_885(CardRegistry& r) {
    r.registerCard(885, std::make_unique<Stargazer>());
}

}  // namespace riftbound
