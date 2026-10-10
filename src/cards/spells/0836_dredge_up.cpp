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

class DredgeUp : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Draw 1." / "[Flow] [2]"
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.drawCards(ctx.controller, 1);
    }
    FlowCost flowCost() const override { return flow(2); }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 836;
        d.def_id = R"RB(ven-049-166)RB";
        d.name = R"RB(Dredge Up)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-049/166)RB";
        d.collector_number = 49;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Mind};
        d.energy_cost = 2;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Flow);
        d.ability_text = R"RB(Draw 1.
[Flow] [2] (You may play this from your trash for its Flow cost. Then banish it.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/cf561c8a0f8debde66b66d3e74ce861d0ce388ae-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_836(CardRegistry& r) {
    r.registerCard(836, std::make_unique<DredgeUp>());
}

}  // namespace riftbound
