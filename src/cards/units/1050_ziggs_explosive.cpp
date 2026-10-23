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

// Ziggs, Explosive (rad-112-167), Chaos champion unit, 4 energy, 4 Might.
// "[1], [E]: Play a Bomb gear token to a battlefield.
//  When I defend, you may kill a friendly gear to give me +2 [M] this turn."
class ZiggsExplosive : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        return ActivationCost{.exhaust = true, .energy = 1};
    }
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 0};
    }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto picked = pickTarget(ctx, "Ziggs, Explosive: battlefield for a Bomb",
                                 battlefieldCardObjects(ctx.state));
        if (picked == kInvalidId) return;
        if (auto bf = battlefieldIdOfCardObject(ctx.state, picked)) playBombToken(ctx, *bf);
    }

    TriggerType triggerType() const override { return TriggerType::WhenIDefend; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto friendly_gear = [&]() {
            std::vector<GameObjectId> out;
            for (const auto& [id, obj] : ctx.state.objects)
                if (obj.isGear() && obj.controller == ctx.controller && obj.location.has_value())
                    out.push_back(id);
            return out;
        };
        int go = confirmOptional(ctx, "Ziggs, Explosive: kill a friendly gear for +2 Might?",
                                 [&]() { return ctx.state.objectExists(ctx.source) &&
                                                !friendly_gear().empty(); });
        if (go <= 0) return;   // -1 suspended, 0 declined/illegal
        auto gear = pickTarget(ctx, "Ziggs, Explosive: friendly gear to kill", friendly_gear());
        if (gear == kInvalidId) return;
        if (!ctx.state.objectExists(gear) || !ctx.state.objectExists(ctx.source)) return;
        ctx.executor.killObject(gear);
        ctx.executor.giveTemporaryMight(ctx.source, 2);
    }

private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 1050;
        d.def_id = R"RB(rad-112-167)RB";
        d.name = R"RB(Ziggs, Explosive)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-112/167)RB";
        d.collector_number = 112;
        d.artist = R"RB(Yeon Art Studio)RB";
        d.card_type = CardType::Unit;
        d.super_type = SuperType::Champion;
        d.domains = {Domain::Chaos};
        d.tags = {R"RB(Ziggs)RB", R"RB(Zaun)RB", R"RB(Yordle)RB"};
        d.energy_cost = 4;
        d.might = 4;
        d.rarity = Rarity::Rare;
        d.ability_text = R"RB([1], [E]: Play a Bomb gear token to a battlefield. (It has [Deploy]. When it dies, deal 2 to an enemy there and kill your other Bombs there.)
When I defend, you may kill a friendly gear to give me +2 [M] this turn.)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_1050(CardRegistry& r) {
    r.registerCard(1050, std::make_unique<ZiggsExplosive>());
}

}  // namespace riftbound
