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
    // "Use only if you've chosen an enemy unit this turn"
    // ENGINE-GAP: PlayerState::chose_enemy_unit_this_turn is set only by
    // Card::pickTarget (resolve-time single choices). Choices locked in on the
    // Intent at play / activation time (ChainItem::targets of cards that don't
    // use needsPlayTimeTarget) and Card::pickTargetPair picks do not set it;
    // those paths must set the flag too when a chosen object is an enemy unit.
    bool canActivateAbility(const GameState& state, PlayerId controller) const override {
        return state.player(controller).chose_enemy_unit_this_turn;
    }
    // "...and only once each turn" (per Hungry Wolf). The stamp holds
    // turn_number + 1 so the default 0 never matches turn 0.
    // ENGINE-GAP: the stamp is written when the ability RESOLVES (there is no
    // card hook at cost-payment time), so a second activation made while the
    // first is still on the chain is not blocked. Needed: a per-source
    // "activated this turn" stamp set by GameEngine when the activation cost
    // is paid.
    bool canActivateAbilityAt(const GameState& state, PlayerId controller,
                              GameObjectId source, int ability_index) const override {
        if (!Card::canActivateAbilityAt(state, controller, source, ability_index)) return false;
        if (!state.objectExists(source)) return false;
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
