#include "cards/card.h"
#include "cards/card_registry.h"
#include "core/game_state.h"
#include "core/events.h"
#include "engine/effect_executor.h"
#include "cards/card_helpers.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace riftbound {
namespace {

class BladeDancer : public LegendCard {
public:
    const CardDef& def() const override { return def_; }

    std::vector<TriggerType> triggerTypes() const override {
        return {TriggerType::WhenYouChooseAFriendlyUnit, TriggerType::WhenIConquer};
    }

    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (ctx.firing_trigger == TriggerType::WhenIConquer) {
            // "you may pay [1] to ready me."
            if (!ctx.state.objectExists(ctx.source)) return;
            auto& self = ctx.state.getObject(ctx.source);
            if (!self.is_exhausted) return;  // already ready, nothing to do
            auto still_legal = [&ctx]() {
                if (!ctx.state.objectExists(ctx.source)) return false;
                if (!ctx.state.getObject(ctx.source).is_exhausted) return false;
                // CR 445.2 / 164.2.a: floating Energy or a ready rune can pay.
                return payableEnergy(ctx.state, ctx.controller) >= 1;
            };
            if (!still_legal()) return;
            int conf = confirmOptional(ctx, "Blade Dancer: pay [1] to ready me?",
                                       still_legal);
            if (conf < 1) return;
            if (!payEnergy(ctx, ctx.controller, 1)) return;
            ctx.executor.readyObject(ctx.source);
            ctx.events.logTrace("BLADE DANCER: paid [1] -> ready me");
            return;
        }

        if (ctx.firing_trigger == TriggerType::WhenYouChooseAFriendlyUnit) {
            // "you may exhaust me and pay [A] to ready it." Find ready-able
            // (exhausted) friendly units to ready.
            if (!ctx.state.objectExists(ctx.source)) return;
            auto& self = ctx.state.getObject(ctx.source);
            if (self.is_exhausted) return;  // need to exhaust me as cost
            auto findTargets = [&]() {
                std::vector<GameObjectId> out;
                for (auto& [id, obj] : ctx.state.objects) {
                    if (id == ctx.source) continue;
                    if (obj.controller != ctx.controller) continue;
                    if (!obj.isUnit() || !obj.location.has_value()) continue;
                    if (!obj.is_exhausted) continue;
                    out.push_back(id);
                }
                return out;
            };
            auto still_legal = [&ctx, &findTargets]() {
                if (!ctx.state.objectExists(ctx.source)) return false;
                if (ctx.state.getObject(ctx.source).is_exhausted) return false;
                // [A] = power of any Domain: floating power, or a rune recycled
                // for it (CR 164.2.b) — not only what already floats.
                if (ctx.executor.payableAnyPower(ctx.controller) < 1) return false;
                return !findTargets().empty();
            };
            if (!still_legal()) return;
            int conf = confirmOptional(ctx,
                "Blade Dancer: exhaust me + pay [A] to ready a unit?", still_legal);
            if (conf == -1) return;
            if (conf == 0) return;
            auto target = pickTarget(ctx, "Blade Dancer: ready a friendly unit",
                                     findTargets());
            if (target == kInvalidId) return;
            // Pay [A] once, after the target is known (pickTarget re-enters).
            if (!ctx.executor.payAnyPower(ctx.controller, 1)) return;
            ctx.executor.exhaustObject(ctx.source);
            ctx.executor.readyObject(target);
            ctx.events.logTrace("BLADE DANCER: exhaust me + [A] -> ready " +
                                 ctx.state.getObject(target).name);
            return;
        }
    }

private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 555;
        d.def_id = R"RB(sfd-246-221)RB";
        d.name = R"RB(Blade Dancer)RB";
        d.set_code = R"RB(SFD)RB";
        d.set_name = R"RB(Spiritforged)RB";
        d.public_code = R"RB(SFD-246/221)RB";
        d.collector_number = 246;
        d.artist = R"RB(Grafit Studio)RB";
        d.card_type = CardType::Legend;
        d.domains = {Domain::Calm, Domain::Chaos};
        d.tags = {R"RB(Irelia)RB"};
        d.rarity = Rarity::Showcase;
        d.ability_text = R"RB(When you choose a friendly unit, you may exhaust me and pay [A] to ready it.
When you conquer, you may pay [1] to ready me.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/8258072391bbb8d24e9d6e603c3ba1434979a911-744x1039.png?accountingTag=RB)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_555(CardRegistry& r) {
    r.registerCard(555, std::make_unique<BladeDancer>());
}

} // namespace riftbound
