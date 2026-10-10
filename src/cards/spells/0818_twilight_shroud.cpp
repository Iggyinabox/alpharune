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

class TwilightShroud : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Give a friendly unit +1 [M] this turn. It can't be chosen by enemy
    //  spells and abilities this turn." / "[Flow] [2]"
    // The turn-scoped protection is GameObject::untargetable_by_enemy_this_turn
    // (folded into untargetable_by_enemy every aura pass, cleared at end of turn).
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true, .must_be_friendly = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Twilight Shroud", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.giveTemporaryMight(picked, 1);
        if (!ctx.state.objectExists(picked)) return;
        auto& unit = ctx.state.getObject(picked);
        unit.untargetable_by_enemy_this_turn = true;
        unit.untargetable_by_enemy = true;   // effective now; the aura pass keeps it
    }
    FlowCost flowCost() const override { return flow(2); }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 818;
        d.def_id = R"RB(ven-031-166)RB";
        d.name = R"RB(Twilight Shroud)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-031/166)RB";
        d.collector_number = 31;
        d.artist = R"RB(小牛设计)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Calm};
        d.energy_cost = 1;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Flow);
        d.ability_text = R"RB(Give a friendly unit +1 [M] this turn. It can't be chosen by enemy spells and abilities this turn.
[Flow] [2] (You may play this from your trash for its Flow cost. Then banish it.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/0bd93298503d6b1b2f6faf7e97ab494e6e26db76-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_818(CardRegistry& r) {
    r.registerCard(818, std::make_unique<TwilightShroud>());
}

}  // namespace riftbound
