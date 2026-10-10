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

class HandHammer : public SimpleEquipGear {
public:
    const CardDef& def() const override { return def_; }
    HandHammer() : SimpleEquipGear(Domain::Calm) {}
    // [vendetta-E]
    // "[Equip] [G]" - SimpleEquipGear(Calm).
    // Equipped: "I have +2 [M] while I'm at a battlefield with exactly one
    // other unit you control."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachSelfOnce(*this, state, controller, [&](GameObjectId sid, GameObject& self) {
            if (!self.attached_to.has_value() || !state.objectExists(*self.attached_to)) return;
            auto& bearer = state.getObject(*self.attached_to);
            if (!bearer.isUnit() || !bearer.isAtBattlefield()) return;
            if (otherFriendlyUnitsHere(state, bearer) != 1) return;
            auraMight(bearer, sid, 2);
        });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 814;
        d.def_id = R"RB(ven-027-166)RB";
        d.name = R"RB(Hand Hammer)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-027/166)RB";
        d.collector_number = 27;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Gear;
        d.domains = {Domain::Calm};
        d.tags = {R"RB(Equipment)RB"};
        d.energy_cost = 2;
        d.might_bonus = 1;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Equip);
        d.ability_text = R"RB([Equip] [G] ([G]: Attach this to a unit you control.))RB";
        d.effect_text = R"RB(I have +2 [M] while I'm at a battlefield with exactly one other unit you control.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/524fedb9116db0eb62380c03e0853c342c53daeb-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_814(CardRegistry& r) {
    r.registerCard(814, std::make_unique<HandHammer>());
}

}  // namespace riftbound
