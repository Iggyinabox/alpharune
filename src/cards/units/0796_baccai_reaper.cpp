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

class BaccaiReaper : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // "When I attack, you may pay [R] to give me [Assault 2] this turn."
    TriggerType triggerType() const override { return TriggerType::WhenIAttack; }

    // [R] = one Fury power: floating Fury / universal power first, otherwise a
    // ready Fury rune recycled via the shared payOnePower helper.
    static bool hasReadyFuryRune(const GameState& state, PlayerId p) {
        for (const auto& [id, obj] : state.objects) {
            if (!obj.isRune() || obj.controller != p || obj.is_exhausted) continue;
            if (!obj.location.has_value() ||
                !std::holds_alternative<BaseLocation>(*obj.location)) continue;
            for (auto d : obj.domains) if (d == Domain::Fury) return true;
        }
        return false;
    }
    static bool canPayFury(const GameState& state, PlayerId p) {
        const auto& pool = state.player(p).rune_pool;
        return pool.power[static_cast<int>(Domain::Fury)] > 0 ||
               pool.universal_power > 0 || hasReadyFuryRune(state, p);
    }
    static bool payFury(CardContext& ctx) {
        auto& pool = ctx.state.player(ctx.controller).rune_pool;
        int& fury = pool.power[static_cast<int>(Domain::Fury)];
        if (fury > 0) { --fury; return true; }
        if (pool.universal_power > 0) { --pool.universal_power; return true; }
        return payOnePower(ctx, ctx.controller, Domain::Fury);
    }

    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        auto still_legal = [&]() {
            return ctx.state.objectExists(ctx.source) &&
                   ctx.state.getObject(ctx.source).location.has_value() &&
                   canPayFury(ctx.state, ctx.controller);
        };
        int conf = confirmOptional(ctx, "Baccai Reaper: pay [R] for [Assault 2]?", still_legal);
        if (conf == -1) return;  // waiting for agent
        if (conf < 1) return;    // declined / can't pay
        if (!payFury(ctx)) return;
        ctx.executor.giveTemporaryKeyword(ctx.source, Keyword::Assault, 2);
        ctx.events.logTrace("BACCAI REAPER: paid [R] -> [Assault 2] this turn");
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 796;
        d.def_id = R"RB(ven-009-166)RB";
        d.name = R"RB(Baccai Reaper)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-009/166)RB";
        d.collector_number = 9;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Fury};
        d.tags = {R"RB(Shurima)RB"};
        d.energy_cost = 3;
        d.power_cost = 1;
        d.might = 4;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB(When I attack, you may pay [R] to give me [Assault 2] this turn. (+2 [M] while I'm an attacker.))RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/14c473be38b3ec40503f44555db9a4168a45bfb4-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_796(CardRegistry& r) {
    r.registerCard(796, std::make_unique<BaccaiReaper>());
}

}  // namespace riftbound
