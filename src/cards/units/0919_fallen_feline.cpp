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

class FallenFeline : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "When you play me, name a spell."
    // The named spell is stored in string_state["named_spell"].
    // ENGINE-GAP: there is no free-form "name a card" decision (MakeChoice
    // only carries a GameObjectId or a small int, not an arbitrary CardDefId),
    // so the nameable set is the distinct spell names the controller can see
    // an opponent's copy of: spells in the opponent's trash and banishment and
    // opponent spells on the chain. Needed: a MakeChoice variant keyed by
    // CardDefId over every spell in CardDB. With no such spell visible nothing
    // is named.
    static constexpr const char* kNamedKey = "named_spell";
    TriggerType triggerType() const override { return TriggerType::WhenYouPlayMe; }
    static std::vector<GameObjectId> nameableSpells(const GameState& state, PlayerId opp) {
        std::vector<GameObjectId> out;
        std::vector<std::string> seen;
        auto consider = [&](GameObjectId cid) {
            if (!state.objectExists(cid)) return;
            const auto& obj = state.getObject(cid);
            if (!obj.isSpell() || obj.name.empty()) return;
            if (std::find(seen.begin(), seen.end(), obj.name) != seen.end()) return;
            seen.push_back(obj.name);
            out.push_back(cid);
        };
        for (auto cid : state.player(opp).trash) consider(cid);
        for (auto cid : state.player(opp).banishment) consider(cid);
        for (const auto& item : state.chain.items)
            if (item.is_spell && item.controller == opp) consider(item.source);
        return out;
    }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        if (!ctx.state.objectExists(ctx.source)) return;
        GameObjectId picked = pickTarget(ctx, "Fallen Feline: name a spell",
                                         nameableSpells(ctx.state, opponent(ctx.controller)));
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        const std::string name = ctx.state.getObject(picked).name;
        ctx.state.getObject(ctx.source).string_state[kNamedKey] = name;
        ctx.events.logTrace("FALLEN FELINE: named \"" + name + "\"");
    }
    // "While I'm at a battlefield, opponents can't play spells with that name."
    // locked_spell_names is cleared every aura pass and re-asserted here.
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachSelfOnce(*this, state, controller, [&](GameObjectId, GameObject& self) {
            if (!self.isAtBattlefield()) return;
            auto it = self.string_state.find(kNamedKey);
            if (it == self.string_state.end() || it->second.empty()) return;
            auto& locked = state.player(opponent(controller)).locked_spell_names;
            if (std::find(locked.begin(), locked.end(), it->second) == locked.end())
                locked.push_back(it->second);
        });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 919;
        d.def_id = R"RB(ven-132-166)RB";
        d.name = R"RB(Fallen Feline)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-132/166)RB";
        d.collector_number = 132;
        d.artist = R"RB(Dao Trong Le)RB";
        d.card_type = CardType::Unit;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Cat)RB", R"RB(Piltover)RB"};
        d.energy_cost = 2;
        d.power_cost = 1;
        d.might = 3;
        d.rarity = Rarity::Rare;
        d.ability_text = R"RB(When you play me, name a spell.
While I'm at a battlefield, opponents can't play spells with that name.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/e5d46c6d56dc14670b5e25547eeb1ee3ceca5a80-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_919(CardRegistry& r) {
    r.registerCard(919, std::make_unique<FallenFeline>());
}

}  // namespace riftbound
