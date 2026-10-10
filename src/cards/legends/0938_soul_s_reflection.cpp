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

class SoulSReflection : public LegendCard {
public:
    const CardDef& def() const override { return def_; }
    // "When you empower something else, empower me."
    TriggerType triggerType() const override { return TriggerType::WhenYouEmpower; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.empower(ctx.source);
    }
    // "Disempower me, [E]: Give a unit at a battlefield -2 [M] this turn."
    std::vector<ActivatedAbility> activatedAbilities() const override {
        return {{
            .cost = {.exhaust = true, .disempower_self = true},
            .targets = TargetRequirements{.count = 1, .must_be_unit = true,
                                          .must_be_at_battlefield = true},
            .needs_activation_time_target = true,
        }};
    }
    std::vector<GameObjectId> enumerateLegalTargets(
        const GameState& state, PlayerId controller, int /*ability_index*/) const override {
        return chooseableUnits(state, controller,
            [](const GameObject& u) { return u.isAtBattlefield(); });
    }
    void onActivate(CardContext& ctx, int /*ability_index*/,
                    const std::vector<GameObjectId>& targets) override {
        GameObjectId picked = !targets.empty() ? targets[0]
            : pickTarget(ctx, "Soul's Reflection",
                         enumerateLegalTargets(ctx.state, ctx.controller, 0));
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.giveTemporaryMight(picked, -2);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 938;
        d.def_id = R"RB(ven-151-166)RB";
        d.name = R"RB(Soul's Reflection)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-151/166)RB";
        d.collector_number = 151;
        d.artist = R"RB(Envar Studio)RB";
        d.card_type = CardType::Legend;
        d.domains = {Domain::Mind, Domain::Chaos};
        d.tags = {R"RB(Mel)RB"};
        d.rarity = Rarity::Rare;
        d.ability_text = R"RB(When you empower something else, empower me. (I become Empowered if I'm not already.)
Disempower me, [E]: Give a unit at a battlefield -2 [M] this turn.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/2e9e60b3f161b41219c40ef0426049634fb7ba1a-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_938(CardRegistry& r) {
    r.registerCard(938, std::make_unique<SoulSReflection>());
}

}  // namespace riftbound
