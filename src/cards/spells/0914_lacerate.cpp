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

class Lacerate : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // [vendetta-E]
    // "Choose a unit. If it's [Empowered], disempower it. Then kill it if it
    //  has 3 [M] or less."
    // ENGINE-GAP: a card cannot force an aura recalculation mid-resolution.
    // Might granted by the unit's own "[Empowered][>] I have +N [M]" passive
    // is an AuraEffect folded into current_might by
    // GameEngine::recalculateAuras, which only runs in cleanup AFTER this
    // spell has resolved. So the "3 [M] or less" check below still sees the
    // pre-disempower Might for such units. Needed: an EffectExecutor hook
    // (e.g. `recomputeContinuousEffects()` forwarding to
    // GameEngine::recalculateAuras) callable between the disempower and the
    // Might check.
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        std::sort(legal.begin(), legal.end());
        GameObjectId picked = pickTarget(ctx, "Lacerate", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        if (ctx.state.getObject(picked).isEmpowered()) ctx.executor.disempower(picked);
        auto& unit = ctx.state.getObject(picked);
        unit.recomputeMight();
        if (unit.location.has_value() && unit.current_might <= 3)
            ctx.executor.killObject(picked);
    }
    // "[Flow] [4][Y][Y]"
    FlowCost flowCost() const override { return flow(4, 2, Domain::Order); }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 914;
        d.def_id = R"RB(ven-127-166)RB";
        d.name = R"RB(Lacerate)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-127/166)RB";
        d.collector_number = 127;
        d.artist = R"RB(Polar Engine Studio)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Order};
        d.energy_cost = 2;
        d.power_cost = 1;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Flow);
        d.ability_text = R"RB(Choose a unit. If it's [Empowered], disempower it. Then kill it if it has 3 [M] or less.
[Flow] [4][Y][Y] (You may play this from your trash for its Flow cost. Then banish it.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/f21bbd09545d949f095756281e5506128eb4809d-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_914(CardRegistry& r) {
    r.registerCard(914, std::make_unique<Lacerate>());
}

}  // namespace riftbound
