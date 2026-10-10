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

class ReluctantLeader : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "When you play another unit, give me +2 [M] this turn."
    // WhenYouPlayAUnit fires on the controller's OTHER on-board cards, so
    // ctx.source is me and the played unit is never me.
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayAUnit; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        if (!ctx.state.getObject(ctx.source).location.has_value()) return;
        ctx.executor.giveTemporaryMight(ctx.source, 2);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 908;
        d.def_id = R"RB(ven-121-166)RB";
        d.name = R"RB(Reluctant Leader)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-121/166)RB";
        d.collector_number = 121;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Ionia)RB"};
        d.energy_cost = 4;
        d.power_cost = 1;
        d.might = 3;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(When you play another unit, give me +2 [M] this turn.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/d70352ba57ae548c405614ec6ba2bb17d7a19a44-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_908(CardRegistry& r) {
    r.registerCard(908, std::make_unique<ReluctantLeader>());
}

}  // namespace riftbound
