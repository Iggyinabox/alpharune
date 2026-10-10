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

class MelDefiantSoul : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "[Empower] — Discard a spell"
    EmpowerSpec empowerSpec() const override {
        EmpowerSpec s;
        s.valid = true;
        s.cost.discard = true;
        s.cost.discard_count = 1;
        s.cost.discard_type_filter = 1 + static_cast<int>(CardType::Spell);
        return s;
    }
    // "When I become [Empowered], banish an enemy unit at a battlefield with
    //  3 [M] or less."
    TriggerType triggerType() const override { return TriggerType::WhenIBecomeEmpowered; }
    void onTrigger(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = chooseableUnits(ctx.state, ctx.controller, [&](const GameObject& obj) {
            return obj.controller != ctx.controller && obj.isAtBattlefield() &&
                   obj.current_might <= 3;
        });
        GameObjectId picked = pickTarget(ctx, "Mel, Defiant Soul: banish", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.banishObject(picked);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 897;
        d.def_id = R"RB(ven-110-166)RB";
        d.name = R"RB(Mel, Defiant Soul)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-110/166)RB";
        d.collector_number = 110;
        d.artist = R"RB(Pandart Studio)RB";
        d.card_type = CardType::Unit;
        d.super_type = SuperType::Champion;
        d.domains = {Domain::Chaos};
        d.tags = {R"RB(Noxus)RB", R"RB(Mel)RB"};
        d.energy_cost = 5;
        d.might = 4;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] — Discard a spell (Pay the cost: Empower me. Use only if not Empowered.)
When I become [Empowered], banish an enemy unit at a battlefield with 3 [M] or less.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/214567717b4f0156ff06c923e1603234cb52db61-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_897(CardRegistry& r) {
    r.registerCard(897, std::make_unique<MelDefiantSoul>());
}

}  // namespace riftbound
