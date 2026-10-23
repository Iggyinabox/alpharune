#include "cards/card.h"
#include "cards/card_registry.h"
#include "core/game_state.h"
#include "core/events.h"
#include "engine/effect_executor.h"
#include <memory>
#include <vector>
#include "cards/card_helpers.h"

namespace riftbound {
namespace {

// Bilgewater Tracker (rad-003-167), Fury, 3 energy, 3 Might.
// "When you play me, deal 1 to each of up to 2 units."
// Domain per Riot's gallery via Riftwatch (Fury); the 10-10 pre-release pull said Body.
class BilgewaterTracker : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayMe; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto units = [&](GameObjectId exclude) {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if (obj.isUnit() && obj.location.has_value() && id != exclude &&
                    id != ctx.source)
                    out.push_back(id);
            return out;
        };
        auto [a, b] = pickTargetPair(ctx, "Bilgewater Tracker: deal 1",
                                     units(kInvalidId),
                                     [&](GameObjectId first) { return units(first); });
        const bool suspended = (a == kInvalidId || b == kInvalidId) &&
                               ctx.state.chain.resuming.has_value() &&
                               (ctx.state.chain.resuming->resume_point == 10 ||
                                ctx.state.chain.resuming->resume_point == 12);
        if (suspended) return;
        for (auto t : {a, b}) {
            if (t == kInvalidId || !ctx.state.objectExists(t)) continue;
            ctx.executor.dealDamage(t, 1, ctx.source);
        }
        for (auto t : {a, b})
            if (t != kInvalidId && ctx.state.objectExists(t) &&
                ctx.state.getObject(t).hasLethalDamage())
                ctx.executor.killObject(t);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 958;
        d.def_id = R"RB(rad-003-167)RB";
        d.name = R"RB(Bilgewater Tracker)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-003/167)RB";
        d.collector_number = 3;
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Bilgewater)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(When you play me, deal 1 to each of up to 2 units.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_958(CardRegistry& r) {
    r.registerCard(958, std::make_unique<BilgewaterTracker>());
}

}  // namespace riftbound
