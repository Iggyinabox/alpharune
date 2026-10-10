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

class DefenderOfTomorrow : public LegendCard {
public:
    const CardDef& def() const override { return def_; }
    // "[Empower] [2][A][A]" / "[1], [E]: Ready a gear." /
    // "[Empowered][>] [1], [E]: Ready 2 gear."
    // One activation: readies 2 gear while Empowered, otherwise 1 (the
    // Empowered ability strictly dominates the base one at the same cost).
    EmpowerSpec empowerSpec() const override {
        return empowerCost(2, 2, Domain::Mind, /*any_domain=*/true);
    }
    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        ActivationCost c;
        c.exhaust = true;
        c.energy = 1;
        return c;
    }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        int n = 1;
        if (ctx.state.objectExists(ctx.source) &&
            ctx.state.getObject(ctx.source).isEmpowered()) n = 2;
        // Ready the first N exhausted friendly gear (no agent choice yet).
        std::vector<GameObjectId> picks;
        for (auto& [id, obj] : ctx.state.objects) {
            if (static_cast<int>(picks.size()) >= n) break;
            if (!obj.isGear() || obj.controller != ctx.controller) continue;
            if (!obj.location.has_value() || !obj.is_exhausted) continue;
            picks.push_back(id);
        }
        for (auto id : picks) ctx.executor.readyObject(id);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 936;
        d.def_id = R"RB(ven-149-166)RB";
        d.name = R"RB(Defender of Tomorrow)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-149/166)RB";
        d.collector_number = 149;
        d.artist = R"RB(Envar Studio)RB";
        d.card_type = CardType::Legend;
        d.domains = {Domain::Mind, Domain::Body};
        d.tags = {R"RB(Jayce)RB"};
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [2][A][A] ([2][A][A]: Empower me. Use only if not Empowered.)
[1], [E]: Ready a gear.
[Empowered][>] [1], [E]: Ready 2 gear.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/670b7efd367a42beba11c18fb59e240a7c574a57-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_936(CardRegistry& r) {
    r.registerCard(936, std::make_unique<DefenderOfTomorrow>());
}

}  // namespace riftbound
