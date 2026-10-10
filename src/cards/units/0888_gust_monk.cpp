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

class GustMonk : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "You may pay [1] as an additional cost to play me."
    OptionalAdditionalCost optionalAdditionalCost() const override {
        return {/*valid=*/true, /*energy=*/1, /*power=*/0, Domain::Chaos,
                /*any_domain=*/false, /*paid_flag=*/"__gust_monk_paid"};
    }
    // "When you play me, if you paid the additional cost, banish a card from
    //  any trash to give a unit [Assault 2] this turn."
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayMe; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        {
            const auto& self = ctx.state.getObject(ctx.source);
            auto it = self.card_counters.find("__gust_monk_paid");
            if (it == self.card_counters.end() || it->second != 1) return;
        }
        // First pick: the card to banish (either player's trash). Second pick:
        // the unit that gets [Assault 2]. No card to banish -> no effect.
        std::vector<GameObjectId> trash_cards;
        for (auto p : {ctx.controller, opponent(ctx.controller)})
            for (auto cid : ctx.state.player(p).trash)
                if (ctx.state.objectExists(cid)) trash_cards.push_back(cid);
        auto units_fn = [&](GameObjectId) {
            return chooseableUnits(ctx.state, ctx.controller,
                                   [](const GameObject&) { return true; });
        };
        auto [card, unit] = pickTargetPair(ctx, "Gust Monk: banish from trash / [Assault 2]",
                                           trash_cards, units_fn);
        if (card == kInvalidId || unit == kInvalidId) return;   // suspended or fizzled
        if (!ctx.state.objectExists(card) || !ctx.state.objectExists(unit)) return;
        auto& banished = ctx.state.getObject(card);
        auto& owner_ps = ctx.state.player(banished.owner);
        auto it = std::find(owner_ps.trash.begin(), owner_ps.trash.end(), card);
        if (it == owner_ps.trash.end()) return;                 // left the trash meanwhile
        owner_ps.trash.erase(it);
        banished.zone = ZoneType::Banishment;
        banished.location = std::nullopt;
        owner_ps.banishment.push_back(card);
        ctx.events.logTrace("GUST MONK: banished " + banished.name + " from trash");
        if (banished.owner == ctx.controller) {
            // "you banished a card you own" bookkeeping (mirrors the Flow path).
            owner_ps.cards_banished_this_turn++;
            ctx.events.emit(ObjectStateChangedEvent{card, "banished_owned"});
        }
        ctx.executor.giveTemporaryKeyword(unit, Keyword::Assault, 2);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 888;
        d.def_id = R"RB(ven-101-166)RB";
        d.name = R"RB(Gust Monk)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-101/166)RB";
        d.collector_number = 101;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Chaos};
        d.tags = {R"RB(Yordle)RB", R"RB(Bandle City)RB"};
        d.energy_cost = 2;
        d.might = 2;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(You may pay [1] as an additional cost to play me.
When you play me, if you paid the additional cost, banish a card from any trash to give a unit [Assault 2] this turn. (+2 [M] while it's an attacker.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/93a194c0c1f055c0b3bc3c0717cbe488749fed92-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_888(CardRegistry& r) {
    r.registerCard(888, std::make_unique<GustMonk>());
}

}  // namespace riftbound
