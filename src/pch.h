#pragma once
/// @file pch.h
/// Precompiled header for the engine library (see RIFTBOUND_USE_PCH in
/// CMakeLists.txt). It is force-included by the build; nothing includes it by
/// hand, and every source file still lists the headers it uses.
///
/// What belongs here: headers that almost every translation unit parses — the
/// card API and the state/event/executor headers each of the ~960 card files
/// includes, plus the standard and Boost headers those pull in. Parsing these
/// was ~80% of a card file's compile time. A change to any header below still
/// rebuilds everything, but each file then costs codegen only.

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "cards/card.h"
#include "cards/card_helpers.h"
#include "cards/card_registry.h"
#include "core/card_db.h"
#include "core/events.h"
#include "core/game_state.h"
#include "core/intent.h"
#include "core/types.h"
#include "engine/effect_executor.h"
