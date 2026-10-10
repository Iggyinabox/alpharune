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

class ShadySpectacles : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    // [vendetta-E]
    // "[Equip] [1][Y]"
    // "As this is attached to a unit, choose another friendly unit. The
    //  equipped unit becomes a copy of that unit for as long as this is
    //  attached to it."
    // ENGINE-GAP (1): the copy is never undone. Needed: an "on unattach" hook
    // (e.g. Card::onUnattached(ctx, gear, unit), called from
    // EffectExecutor::unattachGear and from the kill/bounce/banish paths that
    // clear `attached_to`) so this gear can restore the bearer's printed
    // traits when it comes off.
    // ENGINE-GAP (2): GameEngine runs onEquip outside the chain (no
    // ChainState::resuming), so pickTarget cannot suspend for the agent and
    // falls back to the first legal unit. Needed: equip activations resolved
    // through the resumable chain loop (as needsEquipTimeTarget assumes).
    bool hasEquipAbility() const override { return true; }
    bool onEquip(CardContext& ctx, GameObjectId unit) override {
        if (!ctx.state.objectExists(unit)) return false;
        if (!standardEquip(ctx, ctx.source, unit, /*energy_cost=*/1, Domain::Order))
            return false;
        std::vector<GameObjectId> others;
        for (const auto& [id, obj] : ctx.state.objects) {
            if (id == unit || !obj.isUnit() || obj.controller != ctx.controller) continue;
            if (!obj.location.has_value()) continue;
            others.push_back(id);
        }
        std::sort(others.begin(), others.end());
        GameObjectId chosen = pickTarget(ctx, "Shady Spectacles: copy which friendly unit?",
                                         others);
        if (chosen != kInvalidId && ctx.state.objectExists(chosen))
            ctx.executor.copyUnit(unit, chosen);
        return true;
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 924;
        d.def_id = R"RB(ven-137-166)RB";
        d.name = R"RB(Shady Spectacles)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-137/166)RB";
        d.collector_number = 137;
        d.artist = R"RB(Polar Engine Studio)RB";
        d.card_type = CardType::Gear;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Equipment)RB"};
        d.energy_cost = 4;
        d.might_bonus = 0;
        d.rarity = Rarity::Epic;
        d.keywords.set(Keyword::Equip);
        d.ability_text = R"RB([Equip] [1][Y] ([1][Y]: Attach this to a unit you control.)
As this is attached to a unit, choose another friendly unit. The equipped unit becomes a copy of that unit for as long as this is attached to it.)RB";
        d.effect_text = R"RB((I am a copy of the chosen unit.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/e49461109a4116c22af9206719f53fb73aee36d0-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_924(CardRegistry& r) {
    r.registerCard(924, std::make_unique<ShadySpectacles>());
}

}  // namespace riftbound
