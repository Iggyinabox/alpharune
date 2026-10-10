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

class DecreeOfInsight : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    // "Ignore [Deflect] while paying this spell's cost.
    //  Give an enemy Body ([O]) unit -5 [M] this turn."
    //
    // CR 809: [Deflect] is a mandatory additional cost on spells and abilities
    // that choose the unit. Card::pickTarget (and the default target
    // enumeration) skip that cost for a card answering ignoresDeflect().
    bool ignoresDeflect() const override { return true; }
    static bool hasDomain(const GameObject& obj, Domain d) {
        return std::find(obj.domains.begin(), obj.domains.end(), d) != obj.domains.end();
    }
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true, .must_be_enemy = true};
    }
    using Card::enumerateLegalTargets;
    std::vector<GameObjectId> enumerateLegalTargets(const GameState& state,
                                                    PlayerId controller) const override {
        return chooseableUnits(state, controller, [&](const GameObject& o) {
            return o.controller != controller && hasDomain(o, Domain::Body);
        });
    }
    bool needsPlayTimeTarget() const override { return true; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& /*targets*/) override {
        auto legal = enumerateLegalTargets(ctx.state, ctx.controller);
        GameObjectId picked = pickTarget(ctx, "Decree of Insight", legal);
        if (picked == kInvalidId || !ctx.state.objectExists(picked)) return;
        ctx.executor.giveTemporaryMight(picked, -5);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 848;
        d.def_id = R"RB(ven-061-166)RB";
        d.name = R"RB(Decree of Insight)RB";
        d.set_code = R"RB(VEN)RB";
        d.set_name = R"RB(Vendetta)RB";
        d.public_code = R"RB(VEN-061/166)RB";
        d.collector_number = 61;
        d.artist = R"RB(Envar Studio)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Mind};
        d.energy_cost = 1;
        d.rarity = Rarity::Rare;
        d.keywords.set(Keyword::Reaction);
        d.ability_text = R"RB([Reaction] (Play any time, even before spells and abilities resolve.)
Ignore [Deflect] while paying this spell's cost.
Give an enemy Body ([O]) unit -5 [M] this turn.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/da019355562b207814be6aec55bccf415bd416df-744x1039.png)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_848(CardRegistry& r) {
    r.registerCard(848, std::make_unique<DecreeOfInsight>());
}

}  // namespace riftbound
