#include "cards/card.h"
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

class VanguardHelm : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    // "When a buffed friendly unit dies, buff another friendly unit."
    // ENGINE-GAP: the "buffed" condition is not checked. The dying unit's
    // last-known buff count exists (GameObject::last_known.buff_count, CR
    // 383.2.c.3.b), but TriggerManager::onUnitDied fires WhenAFriendlyUnitDies
    // without a subject, so this card cannot tell WHICH unit died. Needed:
    // fireTrigger(..., WhenAFriendlyUnitDies, /*subject=*/e.object) so
    // ChainItem::triggering_subject names it. Until then every friendly death
    // triggers this.
    TriggerType triggerType() const override { return TriggerType::WhenAFriendlyUnitDies; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        // Prefer to buff a friendly unit ("if it doesn't have a buff, +1").
        std::vector<GameObjectId> targets;
        for (auto& [id, obj] : ctx.state.objects) {
            if (id == ctx.source) continue;
            if (obj.isUnit() && obj.controller == ctx.controller && obj.location.has_value())
                targets.push_back(id);
        }
        if (targets.empty()) return;
        GameObjectId picked = pickTarget(ctx, "Vanguard Helm (buff a friendly unit)", targets);
        if (picked == kInvalidId && ctx.state.chain.resuming.has_value() &&
            ctx.state.chain.resuming->resume_point == 7) {
            return;  // suspended for agent decision
        }
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        if (ctx.state.getObject(picked).buff_count == 0)
            ctx.executor.buffUnit(picked);
        ctx.events.logTrace("VANGUARD HELM: friendly death -> buff a friendly unit");
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 228;
        d.def_id = R"RB(ogn-228-298)RB";
        d.name = R"RB(Vanguard Helm)RB";
        d.set_code = R"RB(OGN)RB";
        d.set_name = R"RB(Origins)RB";
        d.public_code = R"RB(OGN-228/298)RB";
        d.collector_number = 228;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Gear;
        d.domains = {Domain::Order};
        d.energy_cost = 2;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(When a buffed friendly unit dies, buff another friendly unit. (If it doesn't have a buff, it gets a +1 [M] buff.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/9718c666cbf789a572760c826d0a06b26b787ae5-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_228(CardRegistry& r) {
    r.registerCard(228, std::make_unique<VanguardHelm>());
}

} // namespace riftbound
