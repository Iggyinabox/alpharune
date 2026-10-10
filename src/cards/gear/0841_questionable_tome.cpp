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

class QuestionableTome : public GearCard {
public:
    const CardDef& def() const override { return def_; }
    // [vendetta-E]
    // "[Empower] - [E]"
    EmpowerSpec empowerSpec() const override {
        EmpowerSpec s;
        s.valid = true;
        s.cost.exhaust = true;
        return s;
    }
    // "Disempower this, [1], [E]: Draw 1." (ability 0; Empower is appended as 1)
    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        ActivationCost c;
        c.exhaust = true;
        c.energy = 1;
        c.disempower_self = true;
        return c;
    }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        ctx.executor.drawCards(ctx.controller, 1);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 841;
        d.def_id = R"RB(ven-054-166)RB";
        d.name = R"RB(Questionable Tome)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-054/166)RB";
        d.collector_number = 54;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Gear;
        d.domains = {Domain::Mind};
        d.energy_cost = 3;
        d.rarity = Rarity::Common;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] — [E] (Pay the cost: Empower me. Use only if not Empowered.)
Disempower this, [1], [E]: Draw 1.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/07bb606d7cbcb218496b78a4fef354abb99875dc-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_841(CardRegistry& r) {
    r.registerCard(841, std::make_unique<QuestionableTome>());
}

}  // namespace riftbound
