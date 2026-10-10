#pragma once
/// @file scripted_agent.h
/// A fixed, deterministic policy (v6 task P5.1).
///
/// It exists to be imitated: the supervised sanity check trains a network to
/// copy it and then plays that network against random. The rules are simple
/// on purpose, and a function of the visible state and the legal list only:
///
///   1. Play the most expensive card that can be played.
///   2. Otherwise move to the battlefield the opponent holds most weakly,
///      with as many units as possible.
///   3. Otherwise activate an ability.
///   4. Otherwise answer a prompt: accept optional triggers and costs, say
///      yes, keep the opening hand, take the first option offered.
///   5. Otherwise pass or end the turn.
///
/// Ties go to the earlier legal action. It never concedes.

#include "agent_interface.h"
#include "core/card_db.h"

namespace riftbound {

class ScriptedAgent : public AgentInterface {
public:
    explicit ScriptedAgent(const CardDB& card_db) : card_db_(card_db) {}

    Intent selectAction(const GameState& state,
                        const std::vector<Intent>& legal_actions) override;

    /// Index of the action selectAction would return; -1 for an empty list.
    int selectIndex(const GameState& state,
                    const std::vector<Intent>& legal_actions) const;

private:
    const CardDB& card_db_;
};

} // namespace riftbound
