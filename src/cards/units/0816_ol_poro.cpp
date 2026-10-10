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

class OlPoro : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "I can't be played on your first, second, or third turns."
    // PlayerState::turns_taken is 1 during the player's first turn. The
    // restriction names only YOUR turns, so it doesn't apply on an opponent's.
    bool canBePlayed(const GameState& state, PlayerId player) const override {
        if (state.turn.turn_player != player) return true;
        return state.player(player).turns_taken > 3;
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 816;
        d.def_id = R"RB(ven-029-166)RB";
        d.name = R"RB(Ol' Poro)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-029/166)RB";
        d.collector_number = 29;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Calm};
        d.tags = {R"RB(Poro)RB", R"RB(Bandle City)RB"};
        d.energy_cost = 2;
        d.might = 4;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(I can't be played on your first, second, or third turns.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/267fdc483ff8c740779f23eb8f965105feb030cd-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_816(CardRegistry& r) {
    r.registerCard(816, std::make_unique<OlPoro>());
}

}  // namespace riftbound
