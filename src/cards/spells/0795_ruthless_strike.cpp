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

class RuthlessStrike : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "As an additional cost to play this, you may discard 1. Deal 3 to a unit
    //  at a battlefield. If you paid the additional cost, deal 5 to it instead."
    // (GameEngine::maybePayOptionalAdditionalCost offers the discard and sets
    //  card_counters[kPaidFlag].)
    static constexpr const char* kPaidFlag = "__ruthless_strike_paid";
    OptionalAdditionalCost optionalAdditionalCost() const override {
        OptionalAdditionalCost c;
        c.valid = true;
        c.paid_flag = kPaidFlag;
        c.discard_cards = 1;
        return c;
    }
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true,
                                  .must_be_at_battlefield = true};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Ruthless Strike", legal);
        if (picked == kInvalidId && ctx.state.chain.resuming.has_value() &&
            ctx.state.chain.resuming->resume_point == 7) return;  // waiting for the agent
        bool paid = false;
        if (ctx.state.objectExists(ctx.source)) {
            auto& counters = ctx.state.getObject(ctx.source).card_counters;
            auto it = counters.find(kPaidFlag);
            paid = it != counters.end() && it->second == 1;
            if (it != counters.end()) counters.erase(it);  // one play, one payment
        }
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.dealDamage(picked, paid ? 5 : 3, ctx.source);
        if (ctx.state.objectExists(picked) && ctx.state.getObject(picked).hasLethalDamage())
            ctx.executor.killObject(picked);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 795;
        d.def_id = R"RB(ven-008-166)RB";
        d.name = R"RB(Ruthless Strike)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-008/166)RB";
        d.collector_number = 8;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Fury};
        d.energy_cost = 3;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.)
As an additional cost to play this, you may discard 1.
Deal 3 to a unit at a battlefield. If you paid the additional cost, deal 5 to it instead.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/90293ef9407950d171f5fdd040da289a0f6da487-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_795(CardRegistry& r) {
    r.registerCard(795, std::make_unique<RuthlessStrike>());
}

}  // namespace riftbound
