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

class GangplankNaval : public UnitCard {
public:
    const CardDef& def() const override { return def_; }

    // "[Empower] [O][O]"
    EmpowerSpec empowerSpec() const override { return empowerCost(0, 2, Domain::Body); }
    // ENGINE-GAP: "[Empowered][>] If a spell or ability that chooses me would
    // stun me, give me -[M], or return me to hand, give me +3 [M] instead." is a
    // replacement effect and no hook exists for it. Needed: a Card virtual (e.g.
    // `bool replaceChosenEffect(CardContext&, GameObjectId self)`) consulted by
    // EffectExecutor::stunUnit / stunUnitBy, giveTemporaryMight (amount < 0) and
    // bounceToHand before they apply, called only when the currently resolving
    // chain item (state.chain.resuming) is a spell/ability whose chosen targets
    // include the affected object; when it returns true the executor skips the
    // original effect. This card would return true while Empowered after
    // calling giveTemporaryMight(self, +3). Only the Empower cost is implemented.
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 873;
        d.def_id = R"RB(ven-086-166)RB";
        d.name = R"RB(Gangplank, Naval)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-086/166)RB";
        d.collector_number = 86;
        d.artist = R"RB(Six More Vodka)RB";
        d.card_type = CardType::Unit;
        d.super_type = SuperType::Champion;
        d.domains = {Domain::Body};
        d.tags = {R"RB(Bilgewater)RB", R"RB(Gangplank)RB"};
        d.energy_cost = 6;
        d.might = 6;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Empower);
        d.ability_text = R"RB([Empower] [O][O] ([O][O]: Empower me. Use only if not Empowered.)
[Empowered][>] If a spell or ability that chooses me would stun me, give me -[M], or return me to hand, give me +3 [M] instead.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/f270a3ecc4850594513c45d145f2b855152e2cb7-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_873(CardRegistry& r) {
    r.registerCard(873, std::make_unique<GangplankNaval>());
}

}  // namespace riftbound
