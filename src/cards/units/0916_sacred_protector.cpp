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

class SacredProtector : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "I don't deal combat damage unless I'm at a battlefield with exactly one
    //  other unit you control."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachSelfOnce(*this, state, controller, [&](GameObjectId sid, GameObject& self) {
            if (self.isAtBattlefield() && otherFriendlyUnitsHere(state, self) == 1) return;
            GameObject::AuraEffect ae;
            ae.source = sid;
            ae.suppress_combat_damage = true;
            self.aura_effects.push_back(ae);
        });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 916;
        d.def_id = R"RB(ven-129-166)RB";
        d.name = R"RB(Sacred Protector)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-129/166)RB";
        d.collector_number = 129;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Fae)RB", R"RB(Ionia)RB"};
        d.energy_cost = 4;
        d.power_cost = 1;
        d.might = 6;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(I don't deal combat damage unless I'm at a battlefield with exactly one other unit you control.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/a93887af1128d19cdec798611ffdd29900ac5d85-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_916(CardRegistry& r) {
    r.registerCard(916, std::make_unique<SacredProtector>());
}

}  // namespace riftbound
