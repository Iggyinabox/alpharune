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

// Shimmering Swimmer (rad-014-167), Fury, 3 energy, 3 Might.
// "You may kill a friendly unit or gear as an additional cost to play me.
//  When you play me, if you paid the additional cost, deal 2 to a unit."
// APPROXIMATION: OptionalAdditionalCost only models energy/power, so the
// optional sacrifice is taken when the play trigger resolves (yes/no, then
// pick what to kill and the unit to hit). Same outcome barring responses.
class ShimmeringSwimmer : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayMe; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto fodder = [&]() {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if ((obj.isUnit() || obj.isGear()) && obj.controller == ctx.controller &&
                    obj.location.has_value() && id != ctx.source && !obj.attached_to.has_value())
                    out.push_back(id);
            return out;
        };
        auto units = [&](GameObjectId exclude) {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if (obj.isUnit() && obj.location.has_value() && id != exclude)
                    out.push_back(id);
            return out;
        };
        int go = confirmOptional(ctx, "Shimmering Swimmer: kill a friendly unit or gear to deal 2?",
                                 [&]() { return !fodder().empty(); });
        if (go <= 0) return;
        auto [sac, target] = pickTargetPair(ctx, "Shimmering Swimmer", fodder(),
                                            [&](GameObjectId s) { return units(s); });
        const bool suspended = (sac == kInvalidId || target == kInvalidId) &&
                               ctx.state.chain.resuming.has_value() &&
                               (ctx.state.chain.resuming->resume_point == 10 ||
                                ctx.state.chain.resuming->resume_point == 12);
        if (suspended || sac == kInvalidId || !ctx.state.objectExists(sac)) return;
        ctx.executor.killObject(sac);
        if (target == kInvalidId || !ctx.state.objectExists(target)) return;
        if (!ctx.state.getObject(target).location.has_value()) return;
        ctx.executor.dealDamage(target, 2, ctx.source);
        if (ctx.state.objectExists(target) && ctx.state.getObject(target).hasLethalDamage())
            ctx.executor.killObject(target);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 967;
        d.def_id = R"RB(rad-014-167)RB";
        d.name = R"RB(Shimmering Swimmer)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-014/167)RB";
        d.collector_number = 14;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Bilgewater)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(You may kill a friendly unit or gear as an additional cost to play me.
When you play me, if you paid the additional cost, deal 2 to a unit.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_967(CardRegistry& r) {
    r.registerCard(967, std::make_unique<ShimmeringSwimmer>());
}

}  // namespace riftbound
