#include "scripted_agent.h"

#include "core/game_object.h"

#include <variant>

namespace riftbound {

namespace {

bool isPlay(IntentType t) {
    return t == IntentType::PlayCard || t == IntentType::PlayReaction ||
           t == IntentType::PlayActionCard;
}

bool isActivate(IntentType t) {
    return t == IntentType::ActivateAbility ||
           t == IntentType::ActivateReactionAbility ||
           t == IntentType::ActivateActionAbility;
}

/// Total Might of `player`'s units at battlefield `bf`.
int mightAt(const GameState& state, BattlefieldId bf, PlayerId player) {
    int total = 0;
    for (const auto& [id, obj] : state.objects) {
        if (!obj.isUnit() || obj.controller != player || !obj.location) continue;
        const auto* at = std::get_if<BattlefieldLocation>(&*obj.location);
        if (at && at->id == bf && obj.zone == ZoneType::BattlefieldZone)
            total += obj.current_might;
    }
    return total;
}

} // namespace

int ScriptedAgent::selectIndex(const GameState& state,
                               const std::vector<Intent>& legal) const {
    if (legal.empty()) return -1;
    const int n = static_cast<int>(legal.size());

    // 1. The most expensive playable card (Energy, then Power).
    int best = -1, best_cost = -1;
    for (int i = 0; i < n; ++i) {
        const Intent& a = legal[i];
        if (!isPlay(a.type) || !state.objectExists(a.card)) continue;
        const auto def_id = state.getObject(a.card).card_def_id;
        if (def_id == kInvalidId) continue;
        const auto& def = card_db_.get(def_id);
        const int cost = def.energy_cost * 16 + def.power_cost;
        if (cost > best_cost) {
            best_cost = cost;
            best = i;
        }
    }
    if (best >= 0) return best;

    // 2. Move to the battlefield the opponent holds most weakly; among moves
    //    there, the one that brings the most units.
    int best_opp_might = 0, best_units = 0;
    for (int i = 0; i < n; ++i) {
        const Intent& a = legal[i];
        if (a.type != IntentType::StandardMove || !a.move_destination) continue;
        const auto* to = std::get_if<BattlefieldLocation>(&*a.move_destination);
        if (!to) continue;
        const int opp_might = mightAt(state, to->id, opponent(a.player));
        const int units = static_cast<int>(a.units_to_move.size());
        if (best < 0 || opp_might < best_opp_might ||
            (opp_might == best_opp_might && units > best_units)) {
            best = i;
            best_opp_might = opp_might;
            best_units = units;
        }
    }
    if (best >= 0) return best;

    // 3. An ability.
    for (int i = 0; i < n; ++i)
        if (isActivate(legal[i].type)) return i;

    // 4. Prompts.
    for (int i = 0; i < n; ++i) {
        const Intent& a = legal[i];
        if (a.type == IntentType::PlaceOptionalTrigger ||
            a.type == IntentType::PayTriggeredCost)
            return i;
    }
    for (int i = 0; i < n; ++i) {
        const Intent& a = legal[i];
        // Yes over no; for other integer choices the first is as good as any.
        if (a.type == IntentType::MakeChoice && a.chosen_objects.empty() &&
            a.chosen_value && *a.chosen_value == 1)
            return i;
    }
    for (int i = 0; i < n; ++i) {
        const Intent& a = legal[i];
        if (a.type == IntentType::MulliganDecision && a.cards_to_mulligan.empty())
            return i;
    }
    for (int i = 0; i < n; ++i) {
        const IntentType t = legal[i].type;
        if (t == IntentType::MakeChoice || t == IntentType::AssignCombatDamage ||
            t == IntentType::ChooseBattlefield || t == IntentType::PlayFirstDecision ||
            t == IntentType::MulliganDecision || t == IntentType::HideCard ||
            t == IntentType::DeclineOptionalTrigger ||
            t == IntentType::DeclineTriggeredCost || t == IntentType::SideboardSwap)
            return i;
    }

    // 5. Pass, end the turn, or whatever is left that is not conceding.
    for (int i = 0; i < n; ++i)
        if (legal[i].type != IntentType::Concede) return i;
    return 0;
}

Intent ScriptedAgent::selectAction(const GameState& state,
                                   const std::vector<Intent>& legal_actions) {
    const int i = selectIndex(state, legal_actions);
    if (i < 0) return Intent::concede(state.turn.turn_player);
    return legal_actions[i];
}

} // namespace riftbound
