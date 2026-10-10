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

// [vendetta-E] local helpers
Intent objChoice(PlayerId p, GameObjectId id) {
    Intent c;
    c.type = IntentType::MakeChoice;
    c.player = p;
    c.chosen_objects = {id};
    return c;
}
GameObjectId takenObject(CardContext& ctx) {
    auto c = ctx.executor.takeChoice();
    return (c && !c->chosen_objects.empty()) ? c->chosen_objects[0] : kInvalidId;
}

Intent valChoice(PlayerId p, int v, const std::string& label) {
    Intent c;
    c.type = IntentType::MakeChoice;
    c.player = p;
    c.chosen_value = v;
    c.choice_label = label;
    return c;
}

int takenValue(CardContext& ctx, int fallback) {
    auto c = ctx.executor.takeChoice();
    return (c && c->chosen_value.has_value()) ? *c->chosen_value : fallback;
}

// Destinations a card effect may move `u` to, int-coded for MakeChoice:
// 0 = its controller's base, 1 + i = battlefield i. Excludes where it is now.
std::vector<int> moveDests(const GameState& state, const GameObject& u) {
    std::vector<int> out;
    if (!u.location.has_value()) return out;
    if (!u.isAtBase()) out.push_back(0);
    for (const auto& bf : state.battlefields) {
        auto here = u.battlefieldId();
        if (here.has_value() && *here == bf.id) continue;
        out.push_back(1 + static_cast<int>(bf.id));
    }
    return out;
}
std::string destLabel(int d) {
    return d <= 0 ? std::string("Base") : "Battlefield #" + std::to_string(d - 1);
}
void moveTo(CardContext& ctx, GameObjectId u, int dest) {
    if (dest <= 0) ctx.executor.moveToBase(u);
    else ctx.executor.moveToBattlefield(u, static_cast<BattlefieldId>(dest - 1));
}

class TwilightStep : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // [vendetta-E]
    // "Move a unit with 3 [M] or less." The agent picks the unit, then where
    // it moves (its controller's base or any other battlefield).
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true, .max_might = 3};
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.chain.resuming.has_value()) return;
        auto& ri = *ctx.state.chain.resuming;
        if (ri.resume_point == 0) {
            auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
            std::sort(legal.begin(), legal.end());
            if (legal.empty()) { ri.resume_point = 3; return; }
            std::vector<Intent> opts;
            for (auto u : legal) opts.push_back(objChoice(ctx.controller, u));
            ctx.executor.requestChoice(ctx.controller, std::move(opts),
                                       "Twilight Step: move a unit with 3 [M] or less");
            ri.resume_point = 1;
            return;
        }
        if (ri.resume_point == 1) {
            GameObjectId u = takenObject(ctx);
            auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
            if (std::find(legal.begin(), legal.end(), u) == legal.end()) {
                ri.resume_point = 3;
                return;
            }
            auto dests = moveDests(ctx.state, ctx.state.getObject(u));
            if (dests.empty()) { ri.resume_point = 3; return; }
            ri.resume_data = {static_cast<int32_t>(u)};
            std::vector<Intent> opts;
            for (int d : dests) opts.push_back(valChoice(ctx.controller, d, destLabel(d)));
            ctx.executor.requestChoice(ctx.controller, std::move(opts),
                                       "Twilight Step: move it where?");
            ri.resume_point = 2;
            return;
        }
        if (ri.resume_point == 2) {
            ri.resume_point = 3;
            if (ri.resume_data.empty()) return;
            auto u = static_cast<GameObjectId>(ri.resume_data[0]);
            if (!ctx.state.objectExists(u)) return;
            auto dests = moveDests(ctx.state, ctx.state.getObject(u));
            if (dests.empty()) return;
            int d = takenValue(ctx, dests.front());
            if (std::find(dests.begin(), dests.end(), d) == dests.end()) d = dests.front();
            moveTo(ctx, u, d);
        }
    }
    // "[Flow] [4][P]"
    FlowCost flowCost() const override { return flow(4, 1, Domain::Chaos); }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 892;
        d.def_id = R"RB(ven-105-166)RB";
        d.name = R"RB(Twilight Step)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-105/166)RB";
        d.collector_number = 105;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Chaos};
        d.energy_cost = 2;
        d.power_cost = 1;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Flow);
        d.ability_text = R"RB(Move a unit with 3 [M] or less.
[Flow] [4][P] (You may play this from your trash for its Flow cost. Then banish it.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/1c5c10009a9fb2605df5db1ff12647a47ac07d98-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_892(CardRegistry& r) {
    r.registerCard(892, std::make_unique<TwilightStep>());
}

}  // namespace riftbound
