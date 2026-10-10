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

class Clairvoyance : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Predict 5]. Draw 2."
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.predict(ctx.controller, 5);
        ctx.executor.drawCards(ctx.controller, 2);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 843;
        d.def_id = R"RB(ven-056-166)RB";
        d.name = R"RB(Clairvoyance)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-056/166)RB";
        d.collector_number = 56;
        d.artist = R"RB(Max Grecke)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Mind};
        d.energy_cost = 7;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Reaction);
        d.ability_text = R"RB([Reaction] (Play any time, even before spells and abilities resolve.)
[Predict 5]. (Look at the top 5 cards of your Main Deck. Recycle any of them and put the rest back in any order.)
Draw 2.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/f43cb85d4939cc6ea1ed67b1c6526eef912cf9a6-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_843(CardRegistry& r) {
    r.registerCard(843, std::make_unique<Clairvoyance>());
}

}  // namespace riftbound
