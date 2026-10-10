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

class HextechDisc : public GearCard {
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
    // "Disempower this, [1], [E]: Play a 3 [M] Mech unit token to your base."
    // (ability 0; Empower is appended as 1)
    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        ActivationCost c;
        c.exhaust = true;
        c.energy = 1;
        c.disempower_self = true;
        return c;
    }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        LocationId loc{BaseLocation{ctx.controller}};
        ctx.executor.createToken(ctx.controller, CardType::Unit, "Mech", 3, {"Mech"},
                                 KeywordSet{}, loc, /*enter_ready=*/false);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 874;
        d.def_id = R"RB(ven-087-166)RB";
        d.name = R"RB(Hextech Disc)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-087/166)RB";
        d.collector_number = 87;
        d.artist = R"RB(Polar Engine Studio)RB";
        d.card_type = CardType::Gear;
        d.domains = {Domain::Body};
        d.energy_cost = 4;
        d.power_cost = 1;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] — [E] (Pay the cost: Empower this. Use only if not Empowered.)
Disempower this, [1], [E]: Play a 3 [M] Mech unit token to your base.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/36acfeb7da10f1254e962b7bacda3e6ea559fe53-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_874(CardRegistry& r) {
    r.registerCard(874, std::make_unique<HextechDisc>());
}

}  // namespace riftbound
