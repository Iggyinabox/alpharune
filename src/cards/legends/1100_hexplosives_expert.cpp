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

// Ziggs, Hexplosives Expert (rad-141-167), Fury + Chaos Legend.
// "[1][A], [E]: Play a Bomb gear token to a battlefield."
class HexplosivesExpert : public LegendCard {
public:
    const CardDef& def() const override { return def_; }
    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        return ActivationCost{.exhaust = true, .energy = 1, .power = 1,
                              .power_any_domain = true};
    }
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 0};
    }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto picked = pickTarget(ctx, "Hexplosives Expert: battlefield for a Bomb",
                                 battlefieldCardObjects(ctx.state));
        if (picked == kInvalidId) return;   // suspended, or no battlefield
        if (auto bf = battlefieldIdOfCardObject(ctx.state, picked)) playBombToken(ctx, *bf);
    }

private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 1100;
        d.def_id = R"RB(rad-141-167)RB";
        d.name = R"RB(Hexplosives Expert)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-141/167)RB";
        d.collector_number = 141;
        d.card_type = CardType::Legend;
        d.domains = {Domain::Fury, Domain::Chaos};
        d.tags = {R"RB(Ziggs)RB"};
        d.rarity = Rarity::Rare;
        d.ability_text = R"RB([1][A], [E]: Play a Bomb gear token to a battlefield. (It has [Deploy]. When it dies, deal 2 to an enemy there and kill your other Bombs there.))RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_1100(CardRegistry& r) {
    r.registerCard(1100, std::make_unique<HexplosivesExpert>());
}

}  // namespace riftbound
