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

// Amateur Demolitionist (rad-001-167), Fury, 2 energy, 1 Might.
// "When you play me, play a Bomb gear token to a battlefield."
// Domain per Riot's gallery via Riftwatch (Fury); the 10-10 pre-release pull said Chaos.
class AmateurDemolitionist : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayMe; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>&) override {
        auto picked = pickTarget(ctx, "Amateur Demolitionist: battlefield for a Bomb",
                                 battlefieldCardObjects(ctx.state));
        if (picked == kInvalidId) return;
        if (auto bf = battlefieldIdOfCardObject(ctx.state, picked)) playBombToken(ctx, *bf);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 956;
        d.def_id = R"RB(rad-001-167)RB";
        d.name = R"RB(Amateur Demolitionist)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-001/167)RB";
        d.collector_number = 1;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Bandle City)RB", R"RB(Yordle)RB"};
        d.energy_cost = 2;
        d.might = 1;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(When you play me, play a Bomb gear token to a battlefield. (It has [Deploy]. When it dies, deal 2 to an enemy there and kill your other Bombs there.))RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_956(CardRegistry& r) {
    r.registerCard(956, std::make_unique<AmateurDemolitionist>());
}

}  // namespace riftbound
