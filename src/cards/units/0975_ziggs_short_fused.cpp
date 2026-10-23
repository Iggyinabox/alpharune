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

// Ziggs, Short-Fused (rad-023-167), Fury champion, 5 energy + 1 Fury, 5 Might.
// "When you play me, play a Bomb gear token to a battlefield.
//  Abilities of friendly gear deal 1 Bonus Damage."
// Bonus applied by gear cards through card_helpers.h friendlyGearBonusDamage
// (currently the Bomb's Deathknell). Whether a Deathknell counts as a gear
// "ability" for this is assumed yes pending a ruling.
class ZiggsShortFused : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    int friendlyGearAbilityBonusDamage() const override { return 1; }
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayMe; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto picked = pickTarget(ctx, "Ziggs, Short-Fused: battlefield for a Bomb",
                                 battlefieldCardObjects(ctx.state));
        if (picked == kInvalidId) return;
        if (auto bf = battlefieldIdOfCardObject(ctx.state, picked)) playBombToken(ctx, *bf);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 975;
        d.def_id = R"RB(rad-023-167)RB";
        d.name = R"RB(Ziggs, Short-Fused)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-023/167)RB";
        d.collector_number = 23;
        d.artist = R"RB(Pixelverse)RB";
        d.card_type = CardType::Unit;
        d.super_type = SuperType::Champion;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Ziggs)RB", R"RB(Zaun)RB", R"RB(Yordle)RB"};
        d.energy_cost = 5;
        d.power_cost = 1;
        d.might = 5;
        d.rarity = Rarity::Epic;
        d.ability_text = R"RB(When you play me, play a Bomb gear token to a battlefield. (It has [Deploy]. When it dies, deal 2 to an enemy there and kill your other Bombs there.)
Abilities of friendly gear deal 1 Bonus Damage.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_975(CardRegistry& r) {
    r.registerCard(975, std::make_unique<ZiggsShortFused>());
}

}  // namespace riftbound
