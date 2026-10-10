# Rules-compliance audit: engine vs 2026-10-09 Core Rules

Date: 2026-10-10. Scope: changes to EXISTING behaviour from the Vendetta (2026-07-24) and
Radiance (2026-10-16) core-rules updates, plus defects found on the way. New keywords
(Empower, Flow, Burn, Deploy, Disarm, Show Off) are tracked in
`docs/card-coverage/set-update-2026-10.md`.

**Status of these findings:** static code reading only. Nothing here was reproduced with a
test or a game. Each item needs a failing test written before it is fixed. Line numbers are
from the uncommitted Vendetta working tree.

Tournament-rules side: the deck validator matches every constructed rule (601); the one
miss, sideboard size 8 → 10 (601.1.c.1), is fixed.

Abbreviations: GE `src/engine/game_engine.cpp`, EE `src/engine/effect_executor.cpp`,
CM `src/engine/chain_manager.cpp`, TM `src/engine/trigger_manager.cpp`,
CH `src/cards/card_helpers.h`.

## Audit table

| # | Item | Rules | Engine | Verdict | Explanation / failing scenario |
|---|---|---|---|---|---|
| 1 | Payment optional | 445.2, 204.3, 356.1.b.2, 420.3.b | `Card::confirmOptional`; EE `playIgnoringCost` | PARTIAL | "You may pay" effects can be declined. About 14 cards (Diana Lunari, Blood Rose, Spirit Wheel, Jax, Vayne, Sunken Temple, Hall of Legends, Ancient Henge, …) check only `rune_pool.energy`, so they cannot exhaust runes and are near-dead. Every effect-driven play goes through `playIgnoringCost`: Cursed Sarcophagus, Promising Future, Harrowing, Soulgorger, Here to Help, Void Rush and Reinforce are free, with no pay or decline. |
| 2 | [C] on multi-domain cards | 135.2.e.5-6 | GE cost path | PARTIAL | Base cost uses the union of the card's domains (compliant). `[A][A]` needs both powers from one domain. `[Repeat] [N][A]` passes `Domain::Count` into `canPayAdditionalCost` (GE ~1531): out-of-bounds read, and no rune ever matches. Shadow (752) codes `[1][A]` as Fury-only. |
| 3 | Damage replacement at assignment | 467.2.c.4.a, .5, .10; 438.5 | GE combat damage | VIOLATES | Lethal is `max(1, might - damage)` and damage is written straight to `damage_marked`, bypassing `EE::dealDamage`. Ki Barrier, Counter Strike, Lotus Trap and Kayn's immunity do nothing in combat. |
| 4 | Token is intrinsic | 185.1 | `GameObject::isToken()`; EE copy | PARTIAL | Tokens stay tokens and cards never become tokens. Token-ness occupies the supertype slot, so a token copy of a Champion is not a Champion. Shady Spectacles copying a plain token invalidates the card's `card_def_id` permanently. |
| 5 | Contested removal | 190.3.a/b.1, 323.7.a, 323.10 | GE `processContestedBattlefields` | VIOLATES | No removal step. P1 moves into P2's battlefield and the unit dies in response: a showdown still opens and `runShowdown` calls `scoreConquer` for the existing controller. Effect arrivals (`moveToBattlefield`, `createToken`, `playIgnoringCost`, `takeControl`) never apply Contested, so no combat starts. |
| 6 | "When I die" last-known info | 383.2.c.3.b, 429.1.a.1.b | EE / GE kill paths; TM | PARTIAL | Only `last_location` and `__died_empowered` are captured. Gear is detached and designation cleared before the trigger is queued. Draven, Audacious treats "died at a battlefield" as "in combat". |
| 7 | "You may" / cost at finalization; once each turn | 383.3.a/b/e, 403.1, 405.2, 204.1.c | CM finalize; `confirmOptional` | VIOLATES | Finalize only flips a status flag. May-choices, "[X] to [Y]" costs, targets and once-per-turn gates all run inside `onTrigger` at resolution. |
| 8 | Expired delayed ability | 359.3.e.16 | GE expiration; TM delayed | PARTIAL | Flag-based durations always apply: Hostile Takeover played during the Ending Step still reverts control at expiration. End-of-turn delayed abilities owned by the non-turn player never fire. |
| 9 | Targeting clarifications | 355.10.b, 359.3.e.9.a, 355.9.a.2 | GE spell gen; `needsPlayTimeTarget` | PARTIAL | Restriction-vs-target not modelled. Hidden Blade ignores `targets` and re-picks from all battlefields. Repulse ignores "no other friendly unit", cannot counter abilities, cannot see resolve-time targets. Pending items not modelled. |
| 10 | Split damage | 355.14 | Alpha Strike (751), Volibear (041) | VIOLATES | Round-robin over all enemy units at resolution: no targets, no player split, ignores untargetable units. |
| 11 | Accelerate | 805.1.b.1, 805.2.b | GE resolvePermanent / cost | PARTIAL | Optional and paid at finalize. Multi-domain units may pay any domain; the rule requires one of the unit's own. Rek'Sai Breacher's grant charges a mandatory +[1] and `__rek_sai_accel` is never read, so the unit enters exhausted. Not offered through `playIgnoringCost`. |
| 12 | "Play" semantics | 420.4.a/.b, 426.1.b | GE play paths; CM; CH `revertCounteredPlay` | VIOLATES | Inverted. Play triggers fire at finalization, so a countered spell still triggers them. `revertCounteredPlay` decrements `cards_played_this_turn`, though countered spells should still count. `resetTurnTracking` runs only for the turn player, so the other player's "this turn" counters are stale. |
| 13 | Battlefield ability control | 190.6 | TM `fireBattlefieldTriggers` | PARTIAL | Controller is whichever player the event names. Abandoned Hall and Forgotten Library appear never dispatched (battlefield objects have no `location`). |
| 14 | "Then banish it"; enters / "as" | 391.3.a, 369.3 | CM resolve; CH counter helper | VIOLATES / PARTIAL | `banish_on_leave_chain` is honoured only on normal resolution: a countered Flow spell goes to trash (or hand with Abandon). `onPlay`-based readiness (Shadow) is overwritten by `resolvePermanent`. |
| 15 | "Activate" a named trigger | 383.4.g | Reckoner's Arena (281) | PARTIAL | Runs units' `onTrigger` inline instead of queuing chain items; shares resume slots; skips gear-granted conquer effects. |
| 16 | Unified lethal damage | 142.4 | GE lethal processing | PARTIAL | Elder Dragon applies only in cleanup, not in assignment, and counts any damage on enemy units, not "your damage". |
| 17 | Copy layer | 446, 479.1 | EE copy | PARTIAL | Token sources copy live `keywords`, including "this turn" grants. Copy is never undone. |
| 18 | Units can't be attached | 143.5, 435.1.h | GE `attachGearToUnit`; ~10 hand-rolled attach sites | NOT MODELLED | Unreachable today (no type-changing effect until Radiance's Orianna). |
| 19 | Negated vs ignored | 359.3.e.6, .14 | per card | PARTIAL | Engine only filters illegal play-time targets; nothing general, and resolve-time targets cannot mistarget. |
| 20 | Requirements / restrictions | 355.17 | GE play-location gen | PARTIAL | Perched Grimwyrm's hook skips the Rockfall Path and Mageseeker Warden checks. Here to Help falls back to base. Dragon Roost is a documented gap. |
| 21 | New choices | 750-755 | Mystic Reversal (080), Rebuttal (939) | NOT MODELLED | Control change only. Targeting triggers read `chain.items.back()` before the played item is added. |
| 22 | Granted "at end of turn" | 359.3.e.16 | — | CANNOT TELL | No existing card grants such text. |
| 23 | Ambush | 822.1.b, 143.4 | GE Ambush gen; CM closed-state play | PARTIAL | Showdown path is correct (location gate, enters exhausted). In a Closed State, CM calls `addSpell` for any `PlayReaction`, so an Ambush unit resolves as a spell and goes to trash. |
| 24 | Burn Out | 432.1.b, 432.3 | EE `burnOut`; GE `drawCards` | VIOLATES | Deck and trash both empty: returns with no point, so the player never loses. Win check ignores Aspirant's Climb. Logic is duplicated in GE, and 27 card sites pop `main_deck` directly. |
| 25 | Cleanup order | 319, 323 | GE `cleanup` | PARTIAL | Missing 323.2.b/c (designation removal), 323.6 (gear recall), 323.10. No cleanup between chain items or between `beginningStep` and `scoringStep`: a battlefield whose last unit was Temporary still scores the Hold. |

## Other rule changes

- Untargetability (756-758): honoured by the default `enumerateLegalTargets`; custom overrides (Hostile Takeover) skip it.
- Multiple card types (177): `GameObject` has a single `card_type`. Not modelled.
- Kayle at max Empower (442.1.c.1): `canActivateAbilityAt` blocks activation; the rule allows it as a no-op.
- Patch-note shorthand says Accelerate's [C] becomes [A] on multi-domain units; CR 805.1.b.1 says one of the unit's domains. The Core Rules win.

## Known gaps (confirmed)

- [Deflect] is never charged; no payment path reads `deflect_value`.
- `bounceToHand` and `banishObject` do not detach gear; `killObject` detaches but leaves `is_rules_text_inactive` set.

## Pre-existing defects found on the way

- Legacy "would die … instead" text scan in `killUnit` runs after the structured hook declines, so The Boss, Unlicensed Armory, Zhonya's, Guardian Angel and Soraka save any friendly unit, free and unlimited.
- `killObject` consults no replacement hooks: spell kills bypass Zhonya's and the rest.
- `ActivateReactionAbility` is ignored in Closed State: Gold and Ancient Henge cannot be used in response.
- `canAfford` never counts recycling a ready rune, so [1][C][C] with two ready runes is unaffordable.
- Floating Power is never spent on base costs.
- Mirror Image creates its token at a base with no player, and its Temporary is revoked at end of turn.

## Suggested fix order

`.cpp`-only unless noted.

1. Closed-State plays (item 23): branch on card type, `addPermanent` with `play_location`.
2. Hold after deaths (item 25): cleanup at the end of `beginningStep`, or require unit presence in `scoringStep`.
3. Legacy death scan: skip cards that define `hasReplacementEffect()`.
4. Combat damage (items 3, 16): one helper for needed-for-lethal and applied damage.
5. Per-turn reset for both players (item 12), with a guard so `scoreConquer` runs only on a controller change.
6. Contested removal and application (item 5).
7. Burn Out (item 24): award the point with an empty trash; one implementation.
8. Play triggers at resolution (item 12). Removing the decrement in `revertCounteredPlay` is a CH change (rebuilds ~283 cards).
9. Leaving the chain (item 14): one shared helper honouring `banish_on_leave_chain`. CH change plus the counter cards.
10. Effect payments (item 1): `payableEnergy` / `payEnergy` in the ~14 cards.
11. Power payment (item 2, 11): any-domain mode for `[A][A]` and Repeat; fix the out-of-bounds read.
12. Paid effect-driven plays (item 1): new executor API. `effect_executor.h` change (full rebuild).
13. Trigger finalization (item 7): new `Card` virtual called from finalize. `card.h` change (full rebuild) plus per-card migration.
14. Remaining cleanup steps (item 25).
