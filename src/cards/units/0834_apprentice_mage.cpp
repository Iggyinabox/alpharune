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

class ApprenticeMage : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Empower] [2]"
    EmpowerSpec empowerSpec() const override { return empowerCost(2); }
    // "When I become [Empowered], [Predict 2]."
    TriggerType triggerType() const override { return TriggerType::WhenIBecomeEmpowered; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.predict(ctx.controller, 2);
    }
    // "[Empowered][>] I have +1 [M]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachEmpoweredSelf(*this, state, controller,
            [](GameObjectId sid, GameObject& self) { auraMight(self, sid, 1); });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 834;
        d.def_id = R"RB(ven-047-166)RB";
        d.name = R"RB(Apprentice Mage)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-047/166)RB";
        d.collector_number = 47;
        d.artist = R"RB(Envar Studio)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Mind};
        d.tags = {R"RB(Bilgewater)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [2] ([2]: Empower me. Use only if not Empowered.)
When I become [Empowered], [Predict 2]. (Look at the top 2 cards of your Main Deck. Recycle any of them and put the rest back in any order.)
[Empowered][>] I have +1 [M].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/6e650b9680864a949299cbe2ef7d06d1d22c7d18-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_834(CardRegistry& r) {
    r.registerCard(834, std::make_unique<ApprenticeMage>());
}

}  // namespace riftbound
