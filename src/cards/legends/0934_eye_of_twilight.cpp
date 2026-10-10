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

class EyeOfTwilight : public LegendCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Action][>] [E]: Give a friendly unit [Tank] this turn."
    std::vector<ActivatedAbility> activatedAbilities() const override {
        return {{
            .cost = {.exhaust = true},
            .targets = TargetRequirements{.count = 1, .must_be_unit = true,
                                          .must_be_friendly = true},
            .is_action = true,
            .needs_activation_time_target = true,
        }};
    }
    std::vector<GameObjectId> enumerateLegalTargets(
        const GameState& state, PlayerId controller, int /*ability_index*/) const override {
        return chooseableUnits(state, controller,
            [controller](const GameObject& u) { return u.controller == controller; });
    }
    void onActivate(CardContext& ctx, int /*ability_index*/,
                    const std::vector<GameObjectId>& targets) override {
        GameObjectId picked = !targets.empty() ? targets[0]
            : pickTarget(ctx, "Eye of Twilight",
                         enumerateLegalTargets(ctx.state, ctx.controller, 0));
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.giveTemporaryKeyword(picked, Keyword::Tank, 0);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 934;
        d.def_id = R"RB(ven-147-166)RB";
        d.name = R"RB(Eye of Twilight)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-147/166)RB";
        d.collector_number = 147;
        d.artist = R"RB(Envar Studio)RB";
        d.card_type = CardType::Legend;
        d.domains = {Domain::Calm, Domain::Order};
        d.tags = {R"RB(Shen)RB"};
        d.rarity = Rarity::Rare;
        d.ability_text = R"RB([Action][>] [E]: Give a friendly unit [Tank] this turn. (It must be assigned combat damage first.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/54388a3124b7e1f0331e5970f952778db5f24500-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_934(CardRegistry& r) {
    r.registerCard(934, std::make_unique<EyeOfTwilight>());
}

}  // namespace riftbound
