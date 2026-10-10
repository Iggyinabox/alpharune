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

class ConsumingCurse : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Deal 2 to a unit at a battlefield. This deals 1 Bonus Damage for each
    //  card with this name in your trash."
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true,
                                  .must_be_at_battlefield = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Consuming Curse", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        // Counted on resolution; the resolving copy itself is not in the trash.
        int bonus = 0;
        for (auto cid : ctx.state.player(ctx.controller).trash) {
            if (cid == ctx.source || !ctx.state.objectExists(cid)) continue;
            if (ctx.state.getObject(cid).name == def().name) ++bonus;
        }
        ctx.executor.dealDamage(picked, 2 + bonus, ctx.source);
        if (ctx.state.objectExists(picked) && ctx.state.getObject(picked).hasLethalDamage())
            ctx.executor.killObject(picked);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 797;
        d.def_id = R"RB(ven-010-166)RB";
        d.name = R"RB(Consuming Curse)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-010/166)RB";
        d.collector_number = 10;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Fury};
        d.energy_cost = 2;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.)
Deal 2 to a unit at a battlefield. This deals 1 Bonus Damage for each card with this name in your trash.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/4254a96c88275f09c2c41653d46702125bc24d9f-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_797(CardRegistry& r) {
    r.registerCard(797, std::make_unique<ConsumingCurse>());
}

}  // namespace riftbound
