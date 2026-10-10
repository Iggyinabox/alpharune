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

class GutturalRoar : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Give a unit +2 [M] this turn. If it's [Empowered], give it +4 [M] this
    //  turn instead."
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Guttural Roar", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.giveTemporaryMight(picked, ctx.state.getObject(picked).isEmpowered() ? 4 : 2);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 859;
        d.def_id = R"RB(ven-072-166)RB";
        d.name = R"RB(Guttural Roar)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-072/166)RB";
        d.collector_number = 72;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Body};
        d.energy_cost = 2;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.)
Give a unit +2 [M] this turn. If it's [Empowered], give it +4 [M] this turn instead.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/891f0f85b039b16593865c74b90b0640a312e59b-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_859(CardRegistry& r) {
    r.registerCard(859, std::make_unique<GutturalRoar>());
}

}  // namespace riftbound
