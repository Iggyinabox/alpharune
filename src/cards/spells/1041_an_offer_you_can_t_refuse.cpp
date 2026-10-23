#include "cards/card.h"
#include "cards/card_registry.h"
#include "core/game_state.h"
#include "core/events.h"
#include "engine/effect_executor.h"
#include <memory>
#include <vector>
#include "cards/card_helpers.h"

namespace riftbound {
namespace {

// An Offer You Can't Refuse (rad-101-167), Chaos spell, 2 energy.
// "[Action] Kill a gear. Its controller plays a Gold gear token exhausted."
class AnOfferYouCantRefuse : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>&) override {
        std::vector<GameObjectId> gear;
        for (const auto& [id, obj] : ctx.state.objects)
            if (obj.isGear() && !obj.isUnit() && obj.location.has_value()) gear.push_back(id);
        auto t = pickTarget(ctx, "An Offer You Can't Refuse: gear to kill", gear);
        if (t == kInvalidId || !ctx.state.objectExists(t)) return;
        const PlayerId gear_controller = ctx.state.getObject(t).controller;
        ctx.executor.killObject(t);
        CardContext for_them{ctx.state, ctx.events, ctx.executor, gear_controller, ctx.source,
                             ctx.firing_trigger, ctx.registry};
        createGoldExhausted(for_them);
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 1041;
        d.def_id = R"RB(rad-101-167)RB";
        d.name = R"RB(An Offer You Can't Refuse)RB";
        d.set_code = R"RB(RAD)RB";
        d.set_name = R"RB(Radiance)RB";
        d.public_code = R"RB(RAD-101/167)RB";
        d.collector_number = 101;
        d.artist = R"RB(Kudos Productions)RB";
        d.card_type = CardType::Spell;
        d.domains = {Domain::Chaos};
        d.energy_cost = 2;
        d.rarity = Rarity::Uncommon;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.) Kill a gear. Its controller plays a Gold gear token exhausted. (It has "[Reaction][>] Kill this, [E]: [Add] [A]."))RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_1041(CardRegistry& r) {
    r.registerCard(1041, std::make_unique<AnOfferYouCantRefuse>());
}

}  // namespace riftbound
