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

class Onslaught : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Give a unit +6 [M] this turn." / "[Flow] [4]"
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Onslaught", legal);
        if (picked == kInvalidId) return;
        ctx.executor.giveTemporaryMight(picked, 6);
    }
    FlowCost flowCost() const override { return flow(4); }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 868;
        d.def_id = R"RB(ven-081-166)RB";
        d.name = R"RB(Onslaught)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-081/166)RB";
        d.collector_number = 81;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Body};
        d.energy_cost = 4;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Flow);
        d.ability_text = R"RB(Give a unit +6 [M] this turn.
[Flow] [4] (You may play this from your trash for its Flow cost. Then banish it.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/6a1ef90c64e07b3f561e34c8d43ff5f64db925d8-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_868(CardRegistry& r) {
    r.registerCard(868, std::make_unique<Onslaught>());
}

}  // namespace riftbound
