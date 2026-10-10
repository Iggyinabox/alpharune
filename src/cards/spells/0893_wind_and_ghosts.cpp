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

class WindAndGhosts : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // [vendetta-E]
    // "Choose a unit at a battlefield. If it has 3 [M] or less, banish it.
    //  Otherwise, return it to its owner's hand."
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true,
                                  .must_be_at_battlefield = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        std::sort(legal.begin(), legal.end());
        GameObjectId picked = pickTarget(ctx, "Wind and Ghosts", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        auto& unit = ctx.state.getObject(picked);
        if (!unit.location.has_value()) return;
        // Gear attached to a unit that leaves the board stays behind unattached.
        for (auto gid : std::vector<GameObjectId>(unit.attachments))
            ctx.executor.unattachGear(gid);
        if (unit.current_might <= 3) ctx.executor.banishObject(picked);
        else ctx.executor.bounceToHand(picked);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 893;
        d.def_id = R"RB(ven-106-166)RB";
        d.name = R"RB(Wind and Ghosts)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-106/166)RB";
        d.collector_number = 106;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Chaos};
        d.energy_cost = 3;
        d.power_cost = 1;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.)
Choose a unit at a battlefield. If it has 3 [M] or less, banish it. Otherwise, return it to its owner's hand.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/ef118660422af84f6183d375b68ce1872acb1e2e-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_893(CardRegistry& r) {
    r.registerCard(893, std::make_unique<WindAndGhosts>());
}

}  // namespace riftbound
