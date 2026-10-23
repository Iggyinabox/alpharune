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

// Explosimian (rad-094-167), Chaos, 6 energy, 6 Might.
// "When I hold, play three Bomb gear tokens to a single battlefield."
class Explosimian : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    TriggerType triggerType() const override { return TriggerType::WhenIHold; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto picked = pickTarget(ctx, "Explosimian: battlefield for three Bombs",
                                 battlefieldCardObjects(ctx.state));
        if (picked == kInvalidId) return;
        if (auto bf = battlefieldIdOfCardObject(ctx.state, picked))
            for (int i = 0; i < 3; ++i) playBombToken(ctx, *bf);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 1101;
        d.def_id = R"RB(rad-094-167)RB";
        d.name = R"RB(Explosimian)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-094/167)RB";
        d.collector_number = 94;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Chaos};
        d.tags = {R"RB(Zaun)RB"};
        d.energy_cost = 6;
        d.might = 6;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(When I hold, play three Bomb gear tokens to a single battlefield. (They have [Deploy]. When each dies, deal 2 to an enemy there and kill your other Bombs there.))RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_1101(CardRegistry& r) {
    r.registerCard(1101, std::make_unique<Explosimian>());
}

}  // namespace riftbound
