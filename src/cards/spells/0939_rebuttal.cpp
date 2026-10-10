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

// [vendetta-E] local helpers
// "[A]" = one power of any domain: floating power first, else a rune to recycle.
bool canPayAnyPower(const GameState& state, PlayerId player) {
    if (state.player(player).rune_pool.totalPower() > 0) return true;
    return runesControlled(state, player) > 0;
}
bool payAnyPower(CardContext& ctx, PlayerId player) {
    auto& ps = ctx.state.player(player);
    auto& pool = ps.rune_pool;
    if (pool.universal_power > 0) { --pool.universal_power; return true; }
    for (int d = 0; d < static_cast<int>(Domain::Count); ++d)
        if (pool.power[d] > 0) { --pool.power[d]; return true; }
    // Recycle a rune (an exhausted one if possible, keeping ready runes for energy).
    GameObjectId rune = kInvalidId;
    for (const auto& [id, obj] : ctx.state.objects) {
        if (!obj.isRune() || obj.controller != player || !obj.location.has_value()) continue;
        const bool better = rune == kInvalidId ||
            (obj.is_exhausted && !ctx.state.getObject(rune).is_exhausted) ||
            (obj.is_exhausted == ctx.state.getObject(rune).is_exhausted && id < rune);
        if (better) rune = id;
    }
    if (rune == kInvalidId) return false;
    auto& r = ctx.state.getObject(rune);
    ctx.events.logTrace("  RECYCLE_RUNE: " + r.name + " for [A] (Rebuttal)");
    r.location = std::nullopt;
    r.zone = ZoneType::RuneDeck;
    r.is_exhausted = false;
    ps.rune_deck.insert(ps.rune_deck.begin(), rune);
    ps.power_spent_this_turn++;
    ctx.events.emit(LeftBoardEvent{rune, player, CardType::Rune, BaseLocation{player},
                                   ZoneType::RuneDeck, false});
    return true;
}

class Rebuttal : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // [vendetta-E]
    // "Choose a spell with Energy cost no more than [4]. You may pay [A]. If
    //  you do, gain control of it and you may make new choices for it.
    //  Otherwise, counter it."
    // Peek-and-pop: Rebuttal has already been popped by FEPR, so the spell it
    // answers is the top of the chain. It never scans or flags the chain.
    bool hasLegalTargets(const GameState& state, PlayerId /*controller*/) const override {
        for (auto it = state.chain.items.rbegin(); it != state.chain.items.rend(); ++it)
            if (it->is_spell) return true;
        return false;
    }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        // The chosen spell must still be on top and cost no more than [4]
        // (printed Energy cost, as Defy reads it).
        auto top_ok = [&]() {
            if (ctx.state.chain.items.empty()) return false;
            const auto& top = ctx.state.chain.items.back();
            if (!top.is_spell) return false;
            const int cost = top.card_def_id != kInvalidId
                ? ctx.executor.cardDB().get(top.card_def_id).energy_cost : 0;
            return cost <= 4;
        };
        if (!top_ok()) {
            ctx.events.logTrace("REBUTTAL: no spell costing [4] or less on top of the chain");
            return;
        }
        const int pay = confirmOptional(ctx, "Rebuttal: pay [A] to gain control of the spell",
            [&]() { return top_ok() && canPayAnyPower(ctx.state, ctx.controller); });
        if (pay == -1) return;   // waiting on the agent
        if (!top_ok()) return;
        auto& top = ctx.state.chain.items.back();
        if (pay == 1 && payAnyPower(ctx, ctx.controller)) {
            // Gain control: the spell now resolves for Rebuttal's controller.
            top.controller = ctx.controller;
            std::string name = "spell";
            if (ctx.state.objectExists(top.source)) {
                auto& obj = ctx.state.getObject(top.source);
                obj.controller = ctx.controller;
                name = obj.name;
            }
            // "You may make new choices for it": spells that pick at
            // resolution (needsPlayTimeTarget / pair) re-prompt their NEW
            // controller once any play-time targets are dropped.
            // ENGINE-GAP: spells whose targets were locked into
            // ChainItem::targets at play time keep them - there is no hook
            // to re-run the play-time target selection for a chain item.
            if (ctx.registry && top.card_def_id != kInvalidId) {
                if (const Card* c = ctx.registry->get(top.card_def_id))
                    if (c->needsPlayTimeTarget() || c->needsPlayTimeTargetPair())
                        top.targets.clear();
            }
            ctx.events.logTrace("REBUTTAL: paid [A] - gained control of " + name);
            return;
        }
        // "Otherwise, counter it."
        if (ctx.state.player(top.controller).spells_cant_be_countered) {
            ctx.events.logTrace("REBUTTAL: that spell can't be countered");
            return;
        }
        counterChainTop(ctx);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 939;
        d.def_id = R"RB(ven-152-166)RB";
        d.name = R"RB(Rebuttal)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-152/166)RB";
        d.collector_number = 152;
        d.artist = R"RB(小牛设计)RB";
        d.card_type = CardType::Spell;
        d.super_type = SuperType::Signature;
        d.domains = {Domain::Mind, Domain::Chaos};
        d.tags = {R"RB(Mel)RB"};
        d.energy_cost = 1;
        d.power_cost = 1;
        d.rarity = Rarity::Epic;
        d.keywords.set(Keyword::Reaction);
        d.ability_text = R"RB([Reaction] (Play any time, even before spells and abilities resolve.)
Choose a spell with Energy cost no more than [4]. You may pay [A]. If you do, gain control of it and you may make new choices for it. Otherwise, counter it.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/2df67fe2ba17f0796ff1f9c03b85d2db48fe954b-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_939(CardRegistry& r) {
    r.registerCard(939, std::make_unique<Rebuttal>());
}

}  // namespace riftbound
