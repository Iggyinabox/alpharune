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

class PendulumBlade : public SimpleEquipGear {
public:
    const CardDef& def() const override { return def_; }
    PendulumBlade() : SimpleEquipGear(Domain::Fury) {}
    // [vendetta-E]
    // "[Equip] [R]" - SimpleEquipGear(Fury).
    // Equipped: "When I move to a battlefield, give me +2 [M] this turn."
    TriggerType equippedTriggerType() const override { return TriggerType::WhenIMoveToFB; }
    void onEquippedTrigger(CardContext& ctx, GameObjectId unit,
                           const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(unit)) return;
        ctx.executor.giveTemporaryMight(unit, 2);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 798;
        d.def_id = R"RB(ven-011-166)RB";
        d.name = R"RB(Pendulum Blade)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-011/166)RB";
        d.collector_number = 11;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Gear;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Equipment)RB"};
        d.energy_cost = 3;
        d.might_bonus = 1;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Equip);
        d.ability_text = R"RB([Equip] [R] ([R]: Attach this to a unit you control.))RB";
        d.effect_text = R"RB(When I move to a battlefield, give me +2 [M] this turn.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/857a5cee28f403454b1825dd112c07e9ac06b8a4-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_798(CardRegistry& r) {
    r.registerCard(798, std::make_unique<PendulumBlade>());
}

}  // namespace riftbound
