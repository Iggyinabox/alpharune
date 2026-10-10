# Plan: unity builds per card group, with a shared card PCH

Status: proposed, nothing implemented. Written 2026-10-10 on branch `vendetta-rules-update`.

## Goal

Cut the cost of the most common slow rebuild: editing a card-facing header
(`game_state.h`, `effect_types.h`, `game_object.h`, `card.h`,
`effect_executor.h`), which today recompiles all 955 card translation units.

Cards rarely change once stable, so the usual unity-build downside (editing
one file recompiles its batch) is almost never paid here.

## Current state

- 955 of ~1,060 TUs are cards, all compiled into the single `riftbound_core`
  library via `RIFTBOUND_CARD_SOURCES` (globbed, `CONFIGURE_DEPENDS`).
- Every card includes the same five hub headers. Those headers changed in
  7–14 of the last 150 commits each.
- Uncommitted PCH work exists (`src/pch.h`, `RIFTBOUND_USE_PCH` in
  `CMakeLists.txt`). It makes each recompile cheaper but does not reduce the
  count. This plan builds on it and must land after it.
- `build/` is Release (`-O3`), ccache launcher, mold linker, CMake 3.28, `-j6`.

## Target design

Six OBJECT libraries, one per card directory, each with `UNITY_BUILD ON`,
folded back into `riftbound_core` with `$<TARGET_OBJECTS:...>`.

| Target | Files | Units at batch 16 |
|---|---|---|
| `riftbound_cards_units` | 487 | 31 |
| `riftbound_cards_spells` | 233 | 15 |
| `riftbound_cards_gear` | 114 | 8 |
| `riftbound_cards_battlefields` | 66 | 5 |
| `riftbound_cards_legends` | 49 | 4 |
| `riftbound_cards_runes` | 6 | 1 |
| **Total** | **955** | **64** |

Design decisions:

- **OBJECT libraries, not static libraries.** `cards_init.cpp` calls
  `register_card_N` in the card TUs, and the cards call back into `card.cpp`
  and the executor. Object libraries folded into `riftbound_core` avoid a
  circular static link and leave every consumer (`riftbound`,
  `riftbound_tests`, the `src/openspiel` targets) unchanged.
- **One PCH shared by all six groups.** The groups have identical flags and
  precompile the same headers. One target owns the PCH; the rest use
  `target_precompile_headers(... REUSE_FROM ...)`. Six separate PCHs would
  mean six PCH rebuilds on every hub-header change.
- **Opt-out switch.** `option(RIFTBOUND_UNITY_CARDS ... ON)` so a non-unity
  build dir (e.g. `build-dbg/`) can keep per-file `compile_commands.json`
  entries for clangd.
- **Batch size is a cache variable** (`RIFTBOUND_CARD_UNITY_BATCH`, default
  16) so it can be tuned without editing CMake.

Sketch (per group, wrapped in a function):

```cmake
function(riftbound_add_card_group NAME)
    file(GLOB _srcs CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/cards/${NAME}/*.cpp)
    add_library(riftbound_cards_${NAME} OBJECT ${_srcs})
    target_include_directories(riftbound_cards_${NAME} PRIVATE src)
    target_link_libraries(riftbound_cards_${NAME} PRIVATE
        Boost::boost nlohmann_json::nlohmann_json)
    if(RIFTBOUND_UNITY_CARDS)
        set_target_properties(riftbound_cards_${NAME} PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE ${RIFTBOUND_CARD_UNITY_BATCH})
    endif()
endfunction()
```

## Blockers: name collisions

Unity merges each file's anonymous namespace within a batch. Collisions are
compile errors, not silent misbehaviour. A grep-level scan found the
following; the first unity build may surface a few more.

### Duplicate class names (reprints, 20 names, 40 files)

Units:

| Class | Files |
|---|---|
| `AhriInquisitive` | 0119, 0539 |
| `DaringPoro` | 0210, 0781 |
| `DariusExecutioner` | 0243, 0547 |
| `KarmaChanneler` | 0235, 0548 |
| `LonelyPoro` | 0359, 0777 |
| `MysticPoro` | 0171, 0780 |
| `PlunderingPoro` | 0392, 0778 |
| `PoutyPoro` | 0013, 0776 |
| `SettBrawler` | 0164, 0543 |
| `Sprite` | 0269, 0569 |
| `VayneHunter` | 0035, 0537 |
| `VeteranPoro` | 0421, 0779 |
| `YasuoWindrider` | 0205, 0546 |

Gear:

| Class | Files |
|---|---|
| `Gold` | 0326, 0564 |
| `SealOfDiscord` | 0204, 0545 |
| `SealOfFocus` | 0081, 0538 |
| `SealOfInsight` | 0120, 0541 |
| `SealOfRage` | 0040, 0536 |
| `SealOfStrength` | 0163, 0542 |
| `SealOfUnity` | 0245, 0549 |

Fix: suffix the class with its card id in both files (`KarmaChanneler_235`,
`KarmaChanneler_548`). The class name is file-local; only the
`register_card_N` body references it.

### Duplicate file-local helpers (spells group)

| Helper | Spell files |
|---|---|
| `objChoice`, `takenObject` | 0877, 0890, 0892, 0894, 0927, 0935, 0937, 0943 |
| `valChoice` | 0890, 0892, 0894, 0927, 0935, 0937, 0943 |
| `takenValue` | 0892, 0927, 0935 |
| `moveTo`, `moveDests`, `destLabel` | 0892, 0927 |

Fix: hoist into `src/cards/card_helpers.h` and delete the copies. Confirm
the copies are identical first; if any differ, rename the odd one instead.

Not blocking, but worth hoisting in the same pass:

- `objChoice` / `takenObject` also live in `gear/0854_bottled_constellation.cpp`.
- `splitDamage` and the `kSplit*` constants are duplicated between
  `spells/0751_alpha_strike.cpp` and `units/0041_volibear_furious.cpp`
  (different groups, so no collision).

No `#define`s and no file-scope `using` declarations exist in card files.

## Steps

1. **Baseline.** With the PCH work landed, record wall time for
   `touch src/core/game_state.h && cmake --build build --target riftbound_tests -j6`
   and for a clean build. The other agent's `build/rebuild_pch.log` may
   already cover the first.
2. **Split targets, unity off.** Add the six object libraries and fold them
   into `riftbound_core`. Move the card PCH onto one group with `REUSE_FROM`
   for the rest. Build and run the test suite; behaviour and timing should
   be unchanged. This isolates CMake restructuring from unity effects.
3. **Unity on, offenders excluded.** Turn on `UNITY_BUILD` and mark every
   file listed above with `SKIP_UNITY_BUILD_INCLUSION`. Build, add any newly
   surfaced collisions to the exclusion list, run the tests, measure.
4. **Fix collisions, shrink the exclusion list.** Rename reprint classes,
   hoist the duplicated helpers, remove the exclusions. Build, test.
5. **Tune.** Try batch sizes 8, 16, 32 against the step 1 scenarios. Then
   measure the card groups with and without PCH; if the PCH gain under unity
   is negligible, drop PCH from the card targets (which also removes the
   ccache `-fpch-preprocess` / sloppiness requirement for 90% of the build).
6. **Document.** Update the Build section of `CLAUDE.md` with the new
   targets, the two options, and the rule below.

Each step is one commit and leaves the tree building and tests green.

## Rule for card authors after this lands

File-scope names in a card file must be unique within its group. Put the
card id in the class name when a card is reprinted, and put shared helpers
in `card_helpers.h` instead of copying them.

## Verification

- `RIFTBOUND_ROOT=. ./build/riftbound_tests` passes after every step.
- `./build/riftbound --dump-registry` output is byte-identical before and
  after (confirms all 955 cards still register with the same data).
- Build once with `-DRIFTBOUND_UNITY_CARDS=OFF` to confirm the non-unity
  path still compiles.

## Risks

- **clangd on card files** degrades under unity. Mitigated by the opt-out
  option and a non-unity build dir.
- **Memory per compiler process** grows with batch size at `-O3` under
  WSL2. Start at 16; lower it if the build swaps.
- **`REUSE_FROM` needs identical compile flags** across the groups. Any
  per-group flag later (e.g. lower optimization for cards) must be applied
  to all six or the PCH must be split.
- **New collisions** appear whenever a card file adds a common helper name.
  They fail the build immediately, and the author rule covers them.
- **`CMakeLists.txt` is being edited by the PCH work.** Do not start step 2
  until that is committed.

## Out of scope (follow-ups)

- Lower optimization level for card targets in dev builds.
- Splitting `events.h` (only 53 of 955 cards use it) and narrowing
  `game_state.h` / `effect_executor.h` so fewer header edits reach cards at
  all. Unity makes the full card rebuild cheaper; only this avoids it.
- Splitting `riftbound_tests` into engine and card executables.
