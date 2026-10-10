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

class DiscipleOfShen : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // [Hidden] is engine-handled.
    // "I have [Shield 3] while I'm at a battlefield with exactly one other
    //  unit you control."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachSelfOnce(*this, state, controller, [&](GameObjectId sid, GameObject& self) {
            if (!self.isAtBattlefield()) return;
            if (otherFriendlyUnitsHere(state, self) != 1) return;
            auraKeyword(self, sid, Keyword::Shield, 3);
        });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 904;
        d.def_id = R"RB(ven-117-166)RB";
        d.name = R"RB(Disciple of Shen)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-117/166)RB";
        d.collector_number = 117;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 2;
        d.might = 1;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Hidden);
        d.ability_text = R"RB([Hidden] (Hide now for [A] to react with later for [0].)
I have [Shield 3] while I'm at a battlefield with exactly one other unit you control. (+3 [M] while I'm a defender.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/de5b6316d65297e67a97d12d7c70974f7d3ce7d6-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_904(CardRegistry& r) {
    r.registerCard(904, std::make_unique<DiscipleOfShen>());
}

}  // namespace riftbound
