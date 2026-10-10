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

class HungryWolf : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "[Y]: Ready me and give me +1 [M] this turn. Use only if you've chosen
    //  an enemy unit this turn and only once each turn."
    static constexpr const char* kUsedKey = "__hungry_wolf_used_turn";
    bool hasActivatedAbility() const override { return true; }
    ActivationCost getActivationCost() const override {
        ActivationCost c;
        c.power = 1;
        c.power_domain = Domain::Order;
        return c;
    }
    // "Use only if you've chosen an enemy unit this turn".
    // PlayerState::chose_enemy_unit_this_turn is set by every choice path:
    // play / activation-time targets (ChainManager::addSpell / addAbility) and
    // resolve-time picks (Card::pickTarget / pickTargetPair).
    bool canActivateAbility(const GameState& state, PlayerId controller) const override {
        return state.player(controller).chose_enemy_unit_this_turn;
    }
    // "...and only once each turn" (per Hungry Wolf). The stamp holds
    // turn_number + 1 so the default 0 never matches turn 0.
    // The stamp is written when the ability resolves; an activation of mine
    // that is still on the chain (or resolving) counts as this turn's use too,
    // so a second one can't be stacked on top of it.
    // ENGINE-GAP: an activation that is COUNTERED never resolves and so never
    // stamps — the ability could then be used again that turn. Needed: a
    // per-source "activated this turn" stamp written by GameEngine when the
    // activation cost is paid (PlayerState::abilities_activated_this_turn is
    // per player, not per source).
    static bool isMyPendingActivation(const ChainItem& item, GameObjectId source, int index) {
        return item.is_ability && item.is_activated_ability && item.source == source &&
               item.granted_ability_def == 0 && item.ability_index == index;
    }
    bool canActivateAbilityAt(const GameState& state, PlayerId controller,
                              GameObjectId source, int ability_index) const override {
        if (!Card::canActivateAbilityAt(state, controller, source, ability_index)) return false;
        if (!state.objectExists(source)) return false;
        if (ability_index == 0) {
            for (const auto& item : state.chain.items)
                if (isMyPendingActivation(item, source, 0)) return false;
            if (state.chain.resuming.has_value() &&
                isMyPendingActivation(*state.chain.resuming, source, 0)) return false;
        }
        const auto& self = state.getObject(source);
        auto it = self.card_counters.find(kUsedKey);
        return it == self.card_counters.end() || it->second != state.turn.turn_number + 1;
    }
    void onActivate(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        auto& self = ctx.state.getObject(ctx.source);
        if (!self.location.has_value()) return;
        self.card_counters[kUsedKey] = ctx.state.turn.turn_number + 1;
        ctx.executor.readyObject(ctx.source);
        ctx.executor.giveTemporaryMight(ctx.source, 1);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 912;
        d.def_id = R"RB(ven-125-166)RB";
        d.name = R"RB(Hungry Wolf)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-125/166)RB";
        d.collector_number = 125;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Dog)RB", R"RB(Freljord)RB"};
        d.energy_cost = 4;
        d.might = 4;
        d.rarity = Rarity::Uncommon;
        d.ability_text = R"RB([Y]: Ready me and give me +1 [M] this turn. Use only if you've chosen an enemy unit this turn and only once each turn.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/464b4e2edfd526a3b72a80ab05bc9630b7057c22-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_912(CardRegistry& r) {
    r.registerCard(912, std::make_unique<HungryWolf>());
}

}  // namespace riftbound
