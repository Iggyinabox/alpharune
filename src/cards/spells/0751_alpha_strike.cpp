#include "cards/card.h"
#include "cards/card_registry.h"
#include "core/game_state.h"
#include "core/events.h"
#include "engine/effect_executor.h"
#include "cards/card_helpers.h"
#include "core/intent.h"
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace riftbound {
namespace {

// ── Split damage (CR 355.14) ────────────────────────────────────────────────
// "Deal N damage split among <units>": the controller decides the division at
// resolution (CR 355.14.e), every unit that takes part receives at least 1
// (355.14.f-g), and at most N units can take part (355.14.c). The division is
// built one damage at a time: each prompt assigns the next point to a unit the
// controller picks, which can express every legal split and nothing else.
// Damage is marked only after the whole division is known.
//
// The card is re-entered after every answer, so progress lives on the
// resolving chain item: resume_point kSplitAssign/kSplitAwait/kSplitDone
// (clear of the 0..13 range the Card::pick*/confirmOptional helpers use) and
// resume_data = [total, damage per pick, picked unit, picked unit, ...].
//
// ENGINE-GAP: CR 355.14.a-b make every unit in a split a Target chosen when
// the spell or ability is finalized on the chain. The play/trigger path can
// only lock a fixed TargetRequirements::count of targets, so a variable
// number (1..N) of split targets cannot be declared there; they are chosen at
// resolution instead. Missing: a variable-count ("up to N") target declaration
// in the action generator / Intent / ChainItem::targets. Until then CR
// 355.14.h (dropping targets when the damage shrinks before resolution) can
// not arise, and the units are not "chosen" while the item waits on the chain.
// ENGINE-GAP: ChainManager::stepResolve allows 16 re-entries per resolution
// (kMaxResumeIterations), so at most kSplitMaxPicks prompts are issued; above
// that total each pick assigns ceil(total / kSplitMaxPicks) damage, which
// coarsens the split for a source with more than 12 damage to divide.
// Same code in 0041_volibear_furious.cpp and 0751_alpha_strike.cpp (card
// files only for now; it belongs in card_helpers.h).
constexpr int kSplitAssign = 100;
constexpr int kSplitAwait = 101;
constexpr int kSplitDone = 102;
constexpr int kSplitMaxPicks = 12;

/// Returns true once the damage has been dealt (or there was nothing to do),
/// false while a pick is pending. `legal` are the units the controller may
/// choose; `damaged` receives the units that were dealt damage.
bool splitDamage(CardContext& ctx, const std::string& label, int total,
                 GameObjectId damage_source, std::vector<GameObjectId> legal,
                 std::vector<GameObjectId>& damaged) {
    std::sort(legal.begin(), legal.end());
    std::vector<std::pair<GameObjectId, int>> alloc;
    auto add = [&alloc](GameObjectId id, int n) {
        for (auto& a : alloc) if (a.first == id) { a.second += n; return; }
        alloc.emplace_back(id, n);
    };
    auto deal = [&]() {
        for (const auto& [id, n] : alloc) {
            if (n <= 0 || !ctx.state.objectExists(id)) continue;
            // "If you've chosen an enemy unit this turn" (as Card::pickTarget).
            if (ctx.state.getObject(id).controller != ctx.controller)
                ctx.state.player(ctx.controller).chose_enemy_unit_this_turn = true;
            ctx.events.logTrace("SPLIT: " + label + " -> " + std::to_string(n) +
                                " to " + ctx.state.getObject(id).name);
            ctx.executor.dealDamage(id, n, damage_source);
            damaged.push_back(id);
        }
    };

    auto& ri_opt = ctx.state.chain.resuming;
    if (!ri_opt.has_value()) {
        // Direct invocation (no chain item to resume): no agent can be asked,
        // so default to an even spread over the legal units, like the other
        // choice helpers default to their first option.
        if (total <= 0 || legal.empty()) return true;
        for (int i = 0; i < total; ++i) add(legal[i % legal.size()], 1);
        deal();
        return true;
    }
    auto& ri = *ri_opt;
    if (ri.resume_point == kSplitDone) return true;

    // Deflect (CR 809): choosing an opponent's Deflect unit costs [A] per
    // Deflect, once per unit chosen — charged when the unit first joins the
    // split, the way Card::pickTarget charges it. A unit whose Deflect cost
    // can't be paid is not offered; one already in the split stays available.
    auto alreadyPicked = [&ri](GameObjectId id) {
        for (size_t i = 2; i < ri.resume_data.size(); ++i)
            if (static_cast<GameObjectId>(ri.resume_data[i]) == id) return true;
        return false;
    };
    auto offered = [&]() {
        const int payable = ctx.executor.payableAnyPower(ctx.controller);
        std::vector<GameObjectId> out;
        for (auto t : legal)
            if (alreadyPicked(t) || ctx.executor.deflectCostFor(ctx.controller, t) <= payable)
                out.push_back(t);
        return out;
    };

    if (ri.resume_point == kSplitAwait) {
        auto picked = ctx.executor.takeChoice();
        auto can_pick = offered();
        GameObjectId t = can_pick.empty() ? kInvalidId : can_pick.front();
        if (picked.has_value() && !picked->chosen_objects.empty() &&
            std::find(can_pick.begin(), can_pick.end(),
                      picked->chosen_objects.front()) != can_pick.end())
            t = picked->chosen_objects.front();
        if (t != kInvalidId && !alreadyPicked(t)) {
            const int cost = ctx.executor.deflectCostFor(ctx.controller, t);
            if (cost > 0) {
                ctx.executor.payAnyPower(ctx.controller, cost);
                ctx.events.logTrace("DEFLECT: pays " + std::to_string(cost) +
                                    " [A] to choose " + ctx.state.getObject(t).name);
            }
            ri.chosen_objects.push_back(t);
        }
        ri.resume_data.push_back(static_cast<int32_t>(t));
        ri.resume_point = kSplitAssign;
    } else if (ri.resume_point != kSplitAssign) {
        // First entry: latch the amount to divide.
        if (total <= 0 || legal.empty()) {
            ri.resume_point = kSplitDone;
            return true;
        }
        // One legal unit: a single pick takes everything.
        int per_pick = legal.size() == 1
            ? total : (total + kSplitMaxPicks - 1) / kSplitMaxPicks;
        ri.resume_data = {total, per_pick};
        ri.resume_point = kSplitAssign;
        if (offered().empty()) {   // nothing the controller can afford to choose
            ri.resume_point = kSplitDone;
            return true;
        }
    }

    total = ri.resume_data[0];
    const int per_pick = std::max(1, ri.resume_data[1]);
    int remaining = total;
    for (size_t i = 2; i < ri.resume_data.size() && remaining > 0; ++i) {
        int n = std::min(per_pick, remaining);
        add(static_cast<GameObjectId>(ri.resume_data[i]), n);
        remaining -= n;
    }

    const auto can_pick = remaining > 0 ? offered() : std::vector<GameObjectId>{};
    if (remaining > 0 && !can_pick.empty()) {
        std::vector<Intent> options;
        options.reserve(can_pick.size());
        for (auto t : can_pick) {
            Intent i;
            i.type = IntentType::MakeChoice;
            i.player = ctx.controller;
            i.chosen_objects = { t };
            options.push_back(std::move(i));
        }
        ctx.events.logTrace("SPLIT_PROMPT: " + label + " (" + std::to_string(remaining) +
                            " of " + std::to_string(total) + " damage left to assign)");
        ctx.executor.requestChoice(ctx.controller, std::move(options),
                                   "split damage: " + label + " (" +
                                   std::to_string(remaining) + " left)");
        ri.resume_point = kSplitAwait;
        return false;
    }

    ri.resume_point = kSplitDone;
    deal();
    return true;
}

class AlphaStrike : public SpellCard {
public:
    const CardDef& def() const override { return def_; }
    TargetRequirements getTargetRequirements() const override {
        return TargetRequirements{.count = 1, .must_be_unit = true,
                                  .must_be_friendly = true};
    }
    void onResolve(CardContext& ctx, const std::vector<GameObjectId>& targets) override {
        if (targets.empty() || !ctx.state.objectExists(targets[0])) return;
        GameObjectId src = targets[0];
        // "It deals damage equal to its Might": read as the spell resolves
        // (splitDamage latches it on the first entry).
        int total = ctx.state.getObject(src).current_might;

        // "split among enemy units at battlefields" (CR 355.14): the
        // controller picks the units and the division. Units that can't be
        // chosen by enemy spells and abilities are not legal picks.
        auto legal = chooseableUnits(ctx.state, ctx.controller,
            [&](const GameObject& o) {
                return o.controller != ctx.controller && o.isAtBattlefield();
            });
        std::vector<GameObjectId> damaged;
        if (!splitDamage(ctx, "Alpha Strike", total, src, legal, damaged)) return;

        // Kill lethally-damaged units (after all damage is marked); gain 1 XP
        // per unit this kills.
        int kills = 0;
        for (auto id : damaged) {
            if (!ctx.state.objectExists(id)) continue;
            if (ctx.state.getObject(id).hasLethalDamage()) {
                ctx.executor.killObject(id);
                ++kills;
            }
        }
        if (kills > 0) {
            ctx.state.player(ctx.controller).xp += kills;
            ctx.events.logTrace("ALPHA STRIKE: " + std::to_string(kills) +
                                " kill(s) -> gain " + std::to_string(kills) +
                                " XP (now " +
                                std::to_string(ctx.state.player(ctx.controller).xp) + ")");
        }
    }
private:
    const CardDef def_ = [] {
        CardDef d;
        d.id = 751;
        d.def_id = R"RB(unl-192-219)RB";
        d.name = R"RB(Alpha Strike)RB";
        d.set_code = R"RB(UNL)RB";
        d.set_name = R"RB(Unleashed)RB";
        d.public_code = R"RB(UNL-192/219)RB";
        d.collector_number = 192;
        d.artist = R"RB(莺之歌)RB";
        d.card_type = CardType::Spell;
        d.super_type = SuperType::Signature;
        d.domains = {Domain::Calm, Domain::Body};
        d.tags = {R"RB(Master Yi)RB"};
        d.energy_cost = 3;
        d.power_cost = 1;
        d.rarity = Rarity::Epic;
        d.keywords.set(Keyword::Action);
        d.ability_text = R"RB([Action] (Play on your turn or in showdowns.)
Choose a friendly unit. It deals damage equal to its Might split among enemy units at battlefields. Then for each unit this kills, do this: Gain 1 XP.)RB";
        d.image_url = R"RB(https://cmsassets.rgpub.io/sanity/images/dsfx7636/game_data_live/154940caa774fd8219625cf051ddcb230ffc7f02-744x1039.png?accountingTag=RB)RB";
        return d;
    }();
};

}  // anonymous namespace

void register_card_751(CardRegistry& r) {
    r.registerCard(751, std::make_unique<AlphaStrike>());
}

} // namespace riftbound
