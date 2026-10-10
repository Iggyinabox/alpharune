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

class BrittleSteel : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Kill a gear." / "[Flow] [4][R]"
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_gear = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Brittle Steel: kill a gear", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.unattachGear(picked);   // no-op unless it is equipped
        ctx.executor.killObject(picked);
    }
    FlowCost flowCost() const override { return flow(4, 1, Domain::Fury); }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 790;
        d.def_id = R"RB(ven-003-166)RB";
        d.name = R"RB(Brittle Steel)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-003/166)RB";
        d.collector_number = 3;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Fury};
        d.energy_cost = 2;
        d.power_cost = 1;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Flow);
        d.ability_text = R"RB(Kill a gear.
[Flow] [4][R] (You may play this from your trash for its Flow cost. Then banish it.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/2264ab788558ec69b1b6ed3e03ce69a3f63bdbb7-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_790(CardRegistry& r) {
    r.registerCard(790, std::make_unique<BrittleSteel>());
}

}  // namespace riftbound
