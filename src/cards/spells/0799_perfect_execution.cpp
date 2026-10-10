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

class PerfectExecution : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Ready a unit and give it [Assault 3] this turn." / "[Flow] [3][R]"
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Perfect Execution", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.readyObject(picked);
        ctx.executor.giveTemporaryKeyword(picked, Keyword::Assault, 3);
    }
    FlowCost flowCost() const override { return flow(3, 1, Domain::Fury); }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 799;
        d.def_id = R"RB(ven-012-166)RB";
        d.name = R"RB(Perfect Execution)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-012/166)RB";
        d.collector_number = 12;
        d.artist = R"RB(莺之歌)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Fury};
        d.energy_cost = 3;
        d.power_cost = 1;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Flow);
        d.ability_text = R"RB(Ready a unit and give it [Assault 3] this turn. (+3 [M] while it's an attacker.)
[Flow] [3][R] (You may play this from your trash for its Flow cost. Then banish it.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/cb13c46f8c3872678b87f68891d9f4a416ba8080-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_799(CardRegistry& r) {
    r.registerCard(799, std::make_unique<PerfectExecution>());
}

}  // namespace riftbound
