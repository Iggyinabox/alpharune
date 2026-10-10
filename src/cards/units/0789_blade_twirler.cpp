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

class BladeTwirler : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    unsigned choicesAtFinalization() const override { return kFinalizeTrigger; }
    // "The first time I move each turn, choose a player. They [Burn 1]."
    TriggerType triggerType() const override { return TriggerType::WhenIMove; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        auto& self = ctx.state.getObject(ctx.source);
        // Once per turn: stamp the turn (+1 so 0 = never) on the FIRST entry
        // only; resumed entries (after the player choice) must pass through.
        const int turn_id = ctx.state.turn.turn_number + 1;
        const bool first_entry = !ctx.state.chain.resuming.has_value() ||
                                 ctx.state.chain.resuming->resume_point == 0;
        int& last_fired = self.card_counters["blade_twirler_move_turn"];
        if (first_entry && last_fired == turn_id) return;  // not the first move
        if (first_entry) last_fired = turn_id;

        // Mode 0 = the opponent, mode 1 = yourself.
        int mode = pickMode(ctx, "Blade Twirler: choose a player to [Burn 1]", 2,
                            {"Opponent burns 1", "You burn 1"});
        if (finalizingChoices(ctx)) return;   // choices end here (CR 383.3)
        if (mode < 0) return;  // waiting for agent
        PlayerId who = (mode == 0) ? opponent(ctx.controller) : ctx.controller;
        ctx.executor.burn(who, 1);
        ctx.events.logTrace(std::string("BLADE TWIRLER: first move -> ") +
                            toString(who) + " burns 1");
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 789;
        d.def_id = R"RB(ven-002-166)RB";
        d.name = R"RB(Blade Twirler)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-002/166)RB";
        d.collector_number = 2;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Yordle)RB", R"RB(Ionia)RB"};
        d.energy_cost = 4;
        d.might = 4;
        d.rarity = Rarity::Common;
        d.ability_text = R"RB(The first time I move each turn, choose a player. They [Burn 1]. (They put the top card of their Main Deck into their trash.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/b222082d5d38630c4d2542ed24c0442881e3de18-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_789(CardRegistry& r) {
    r.registerCard(789, std::make_unique<BladeTwirler>());
}

}  // namespace riftbound
