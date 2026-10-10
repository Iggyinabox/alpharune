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
