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

class MournfulWitness : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "When a combat that I was in ends, empower me."
    TriggerType triggerType() const override { return TriggerType::WhenACombatIWasInEnds; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        ctx.executor.empower(ctx.source);  // no-op if already Empowered
    }
    // "[Empowered][>] I have +2 [M]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) { auraMight(self, sid, 2); });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 815;
        d.def_id = R"RB(ven-028-166)RB";
        d.name = R"RB(Mournful Witness)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-028/166)RB";
        d.collector_number = 28;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Calm};
        d.tags = {R"RB(Fae)RB", R"RB(Ionia)RB"};
        d.energy_cost = 2;
        d.might = 2;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(When a combat that I was in ends, empower me. (I become Empowered if I'm not already.)
[Empowered][>] I have +2 [M].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/2f31651fc37a3e8bc562d0a361c509c0998d82c8-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_815(CardRegistry& r) {
    r.registerCard(815, std::make_unique<MournfulWitness>());
}

}  // namespace riftbound
