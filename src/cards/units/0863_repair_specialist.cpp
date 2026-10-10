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

class RepairSpecialist : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "I have [Assault] equal to the number of gear you control."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        const int gear = gearControlled(state, controller);
        if (gear <= 0) return;
        forEachSelfOnce(*this, state, controller,
            [gear](GameObjectId sid, GameObject& self) {
                if (!self.location.has_value()) return;
                auraKeyword(self, sid, Keyword::Assault, gear);
            });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 863;
        d.def_id = R"RB(ven-076-166)RB";
        d.name = R"RB(Repair Specialist)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-076/166)RB";
        d.collector_number = 76;
        d.artist = R"RB(Dao Trong Le)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Body};
        d.tags = {R"RB(Zaun)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(I have [Assault] equal to the number of gear you control. (+1 [M] while I'm an attacker for each instance of Assault.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/b1ef578e372e64afbce717384fb8a3b7fd841fc8-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_863(CardRegistry& r) {
    r.registerCard(863, std::make_unique<RepairSpecialist>());
}

}  // namespace riftbound
