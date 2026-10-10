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

class KayleJustified : public UnitCard {
public:
    const CardDef& def() const override { return def_; }
    // ── behaviour (Vendetta batch C) ──
    // "[Empower] [3] ([3]: Empower me.)" — no "only if not Empowered": the
    // base gate allows it until maxEmpower() statuses are held.
    EmpowerSpec empowerSpec() const override { return empowerCost(3); }
    // "I can be [Empowered] up to three times."
    int maxEmpower() const override { return 3; }
    // "I have +2 [M] for each time I'm [Empowered]."
    // "While I'm [Empowered] three times, I have [Deflect 3] and [Ganking]."
    void applyPassiveAura(GameState& state, PlayerId controller) const override {
        forEachSelfOnce(*this, state, controller, [](GameObjectId sid, GameObject& self) {
            if (self.empowered <= 0) return;
            auraMight(self, sid, 2 * self.empowered);
            if (self.empowered >= 3) {
                auraKeyword(self, sid, Keyword::Deflect, 3);
                auraKeyword(self, sid, Keyword::Ganking);
            }
        });
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 921;
        d.def_id = R"RB(ven-134-166)RB";
        d.name = R"RB(Kayle, Justified)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-134/166)RB";
        d.collector_number = 134;
        d.artist = R"RB(Michal Ivan)RB";
        d.card_type = CardType::Unit;
        d.super_type = SuperType::Champion;
        d.domains = {Domain::Order};
        d.tags = {R"RB(Mount Targon)RB", R"RB(Kayle)RB"};
        d.energy_cost = 3;
        d.might = 3;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [3] ([3]: Empower me.)
I can be [Empowered] up to three times.
I have +2 [M] for each time I'm [Empowered].
While I'm [Empowered] three times, I have [Deflect 3] and [Ganking].)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/8ae883f96bb3a5e20d8002ec49cace430aaf2b88-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_921(CardRegistry& r) {
    r.registerCard(921, std::make_unique<KayleJustified>());
}

}  // namespace riftbound
