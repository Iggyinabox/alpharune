# Set update worklist — Vendetta + Radiance (pulled 2026-10-10)

Generated from `cards/card_index.json` (via `scripts/fetch_cards.py`) against the card classes in `src/cards/`.
Rules sources: `rules/core-rules.md` (2026-10-09), `rules/patch-notes/`, `errata/`.

## Status

| | Vendetta (VEN) | Radiance (RAD) |
|---|---|---|
| Release | 2026-07-31 (rules effective 07-24) | 2026-10-23 (rules effective 10-16) — **pre-release** |
| New units | 92 | 70 |
| New spells | 41 | 33 |
| New gears | 15 | 23 |
| New legends | 9 | 8 |
| New battlefields | 10 | 10 |
| **New cards total** | **167** | **144** |

Not counted: 12 basic-rune reprints and 16 champion reprints whose names already exist in the engine.

Radiance gallery data is pre-release and dirty: 21 cards carried the wrong card type (corrected from the image alt-text by the fetch script) and these have **no domain yet**: Scorched Earth, Antagonistic Apparatus, Entrenched Boar, Hush, Fanfare, Industrious Mechsmith, One-Two Punch, Neeko, Blending In. Re-pull after release before implementing them.

## New mechanics (cards that need each)

| Mechanic | VEN | RAD | Core rule | Engine today |
|---|---|---|---|---|
| Empower | 54 | 1 | 442 + Empowered state | missing |
| Flow | 17 | 0 | keyword (play from trash for Flow cost, then banish) | missing |
| Burn | 8 | 0 | game action (top X of Main Deck to trash) | missing |
| Deploy | 0 | 22 | keyword on gear (lives at a battlefield; killed when an opponent holds) | missing |
| Disarm | 0 | 7 | keyword (attack trigger, -N Might, stacks) | missing |
| Show Off | 0 | 3 | keyword (reveal from hand or pick friendly object; locks info) | missing |
| Skip | 1 | 0 | replacement: skip a turn step | missing |
| Name | 1 | 0 | 759 naming cards/types/tags | missing |
| Ignore | 3 | 0 | 764 ignoring effects | missing |
| Untargetable | 2 | 0 | 756 | partial (`canBeChosenByEnemy`) |
| Pick | 0 | 3 | non-target choice | covered by `pickTarget` helpers (non-target semantics to verify) |
| New choices | 1 | 0 | making new choices (all re-chosen) | exists for Mystic Reversal; needs the Radiance all-or-nothing model |
| Becomes unit | 0 | 1 | 177 multiple types; 143.5 units can't be attached | missing |
| Legend Zone | 0 | 1 | 103.1 extra deckbuilding choice (Neeko) | missing |

New tokens referenced: Recruit, Bomb, Mech, Clone, Tentacle (Gold and Bird already exist).

Rules changes that touch existing engine behavior (see patch notes): energy/power payment is always optional; `[C]` on multi-domain cards must be paid with that card's domains; damage-replacement effects apply at combat damage *assignment*; tokens are an intrinsic category (not a supertype); contested status is removed in cleanup when the applier has no units there; all "When I die" abilities use last-known information; "you may" on triggers is decided at finalization; copy effects get their own layer.

## Decisions taken (2026-10-10)

- **Card vocabulary grown once.** `kCardVocabSize` / `kNumCardDefIds` = 1100: ids 788..955 are Vendetta, 956..1100 are reserved for Radiance. `kStateFeatureDim` = 6188. Existing trainer checkpoints are invalidated.
- **Vendetta now, Radiance after release.** Radiance data is pre-release; re-pull after 2026-10-23 before implementing it.
- **Stock decks.** The 22 lists containing newly banned cards had them swapped for legal cards already in the list (and `Sunken Temple` / `Rockfall Path` for banned battlefields). The swap is a placeholder, not a tuned choice.
- **Ambush.** Per CR 822.1.b it is a play location plus [Reaction] timing; the engine no longer makes Ambush units enter ready.

## Open

- **Existing-card text drift.** ~45 older cards differ between the gallery and the engine beyond templating (e.g. Teemo Strategist, Jax Unmatched, Sona Harmonious, Void Gate, Tianna Crownguard). The gallery lags the errata documents in places, so each needs adjudicating against the errata before changing behavior.
- **[Deflect] costs are not charged** by the engine (the value is tracked only), which predates this update and affects several Vendetta cards that grant or ignore Deflect.

## Vendetta — new cards

| Gallery id | Name | Type | Cost | Might | Domains | New mechanics |
|---|---|---|---|---|---|---|
| ven-001-166 | Baccai Sandspinner | unit (signature) | 6 | 6 | fury | Empower |
| ven-002-166 | Blade Twirler | unit (signature) | 4 | 4 | fury | Burn |
| ven-003-166 | Brittle Steel | spell | 2+1P |  | fury | Flow |
| ven-004-166 | Dune Surfer | unit (signature) | 3 | 3 | fury | Ignore |
| ven-t04 | Recruit | unit |  | 1 |  |  |
| ven-005-166 | Forsaken Baccai | unit (signature) | 2 | 2 | fury |  |
| ven-006-166 | Oasis Raider | unit (signature) | 4 | 4 | fury |  |
| ven-007-166 | Punching Poro | unit | 2 | 2 | fury | Empower |
| ven-008-166 | Ruthless Strike | spell | 3 |  | fury |  |
| ven-009-166 | Baccai Reaper | unit (signature) | 3+1P | 4 | fury |  |
| ven-010-166 | Consuming Curse | spell | 2 |  | fury |  |
| ven-011-166 | Pendulum Blade | gear | 3 | 1 | fury |  |
| ven-012-166 | Perfect Execution | spell | 3+1P |  | fury | Flow |
| ven-013-166 | Shadow Assassin | unit | 5 | 5 | fury |  |
| ven-014-166 | Shadow Fiend | unit | 2 | 2 | fury | Empower |
| ven-015-166 | Decree of Rage | spell | 1+1P |  | fury |  |
| ven-016-166 | Eclipse Dragon | unit | 8 | 8 | fury |  |
| ven-017-166 | Morgana, Vindictive | unit (champion) | 5+1P | 5 | fury |  |
| ven-018-166 | Rage Amplifier | gear | 4+1P |  | fury | Empower |
| ven-019-166 | Renekton, Rage Fueled | unit (champion) | 6 | 6 | fury |  |
| ven-020-166 | Twilight Reveler | unit | 3 | 3 | fury |  |
| ven-021-166 | Akali, Deadly Weapon | unit (champion) | 3 | 3 | fury | Empower |
| ven-022-166 | Endless Riches | gear | 5+1P |  | fury | Burn, Skip |
| ven-023-166 | Zed, From the Shadows | unit (champion) | 4+1P | 4 | fury |  |
| ven-024-166 | Affectionate Poro | unit | 3 | 3 | calm |  |
| ven-025-166 | Esteemed Hierophant | unit (signature) | 5 | 5 | calm |  |
| ven-026-166 | Field Musicians | unit | 4 | 3 | calm |  |
| ven-027-166 | Hand Hammer | gear | 2 | 1 | calm |  |
| ven-028-166 | Mournful Witness | unit | 2 | 2 | calm | Empower |
| ven-029-166 | Ol' Poro | unit | 2 | 4 | calm |  |
| ven-030-166 | Serene Ascetic | unit | 3 | 3 | calm | Empower |
| ven-031-166 | Twilight Shroud | spell | 1 |  | calm | Flow, Untargetable |
| ven-032-166 | Frostcoat Mother | unit (signature) | 3 | 3 | calm | Empower |
| ven-033-166 | Pakaa Protector | unit (signature) | 5 | 4 | calm |  |
| ven-034-166 | Resonating Strike | spell | 2+1P |  | calm |  |
| ven-035-166 | Sanction | spell | 3+1P |  | calm | Empower |
| ven-036-166 | Sandstone Chimera | unit (signature) | 7+2P | 8 | calm |  |
| ven-037-166 | Tomb-Raider Barbara | unit (signature) | 4 | 4 | calm | Empower |
| ven-038-166 | Akali, Silent | unit (champion) | 4+1P | 4 | calm | Untargetable |
| ven-039-166 | Crumbling Sands | spell | 1+1P |  | calm |  |
| ven-040-166 | Decree of Focus | spell | 1 |  | calm |  |
| ven-041-166 | Riven, Shattered | unit (champion) | 3+1P | 3 | calm |  |
| ven-042-166 | Shen, Scourge of Shadows | unit (champion) | 5+1P | 6 | calm |  |
| ven-043-166 | Steel Paws | unit | 1 | 0 | calm | Empower |
| ven-044-166 | Astral Heron | unit | 7 | 7 | calm |  |
| ven-045-166 | Helm of Suppression | gear | 4+1P |  | calm | Empower |
| ven-046-166 | Nasus, Ascended | unit (champion) | 8+1P | 8 | calm | Empower |
| ven-047-166 | Apprentice Mage | unit | 3 | 3 | mind | Empower |
| ven-048-166 | Cloud Drake | unit | 6 | 5 | mind |  |
| ven-049-166 | Dredge Up | spell | 2 |  | mind | Flow |
| ven-050-166 | Grumpy Rockbear | unit (signature) | 4 | 4 | mind | Empower |
| ven-051-166 | Iterative Design | spell | 4 |  | mind | Flow |
| ven-052-166 | Mesmerize | spell | 1+1P |  | mind |  |
| ven-053-166 | Otterpus | unit | 2 | 2 | mind |  |
| ven-054-166 | Questionable Tome | gear | 3 |  | mind | Empower |
| ven-055-166 | Applied Researchers | unit (signature) | 4 | 4 | mind | Empower |
| ven-056-166 | Clairvoyance | spell | 7 |  | mind |  |
| ven-057-166 | Covert Informant | unit | 3+1P | 4 | mind | Empower |
| ven-058-166 | Patched Porobot | unit | 2 | 2 | mind |  |
| ven-059-166 | Shock Blast | spell | 3+1P |  | mind | Empower |
| ven-060-166 | Sky Crusier | unit | 4+1P | 3 | mind |  |
| ven-061-166 | Decree of Insight | spell | 1 |  | mind | Ignore |
| ven-062-166 | Hextech Formula | gear | 2 |  | mind | Empower |
| ven-063-166 | Nasus, Guardian of Knowledge | unit (champion) | 5+1P | 6 | mind |  |
| ven-064-166 | Plaza Guardian | unit | 10 | 8 | mind |  |
| ven-065-166 | Swain, Visionary | unit (champion) | 6+1P | 6 | mind |  |
| ven-066-166 | Temporal Breach | spell | 2+1P |  | mind |  |
| ven-067-166 | Bottled Constellation | gear | 10+2P |  | mind |  |
| ven-068-166 | Jayce, Brilliant Inventor | unit (champion) | 6+1P | 6 | mind |  |
| ven-069-166 | Mel, Newly Awakened | unit (champion) | 4+1P | 4 | mind | Empower |
| ven-070-166 | Brutal Hunter | unit | 3+1P | 4 | body | Empower |
| ven-071-166 | Fretful Feline | unit | 6 | 5 | body |  |
| ven-072-166 | Guttural Roar | spell | 2 |  | body | Empower |
| ven-073-166 | Jagged Cutlass | gear | 3 | 2 | body |  |
| ven-074-166 | Legion Marauder | unit | 2 | 2 | body | Empower |
| ven-075-166 | Platewyrm Egg | gear | 3 |  | body | Empower |
| ven-076-166 | Repair Specialist | unit | 3 | 3 | body |  |
| ven-077-166 | Tools of Empire | gear | 4 |  | body | Empower |
| ven-078-166 | Baccai Witherclaw | unit (signature) | 4 | 4 | body | Empower |
| ven-079-166 | Dame the Despoiler | unit | 5 | 5 | body | Empower |
| ven-080-166 | Noxian Demolitionist | unit | 2 | 1 | body |  |
| ven-081-166 | Onslaught | spell | 4 |  | body | Flow |
| ven-082-166 | Profiteer | unit (signature) | 4 | 4 | body | Empower |
| ven-083-166 | Rampage | spell | 3 |  | body |  |
| ven-084-166 | Ambessa, The Wolf | unit (champion) | 4 | 4 | body | Empower |
| ven-085-166 | Decree of Strength | spell | 1 |  | body |  |
| ven-086-166 | Gangplank, Naval | unit (champion) | 6 | 6 | body | Empower |
| ven-087-166 | Hextech Disc | gear | 4+1P |  | body | Empower |
| ven-088-166 | Jayce, Hammer in Hand | unit (champion) | 4+1P | 5 | body |  |
| ven-089-166 | Wild Claw | spell | 7+1P |  | body | Empower |
| ven-090-166 | Cataclysmic Duel | spell | 8+3P |  | body |  |
| ven-091-166 | Corrupted Dragon | unit | 10+2P | 10 | body |  |
| ven-092-166 | Renekton, Brute | unit (champion) | 5 | 4 | body | Empower |
| ven-093-166 | Kinkou Lifeblade | unit | 4 | 4 | chaos | Empower |
| ven-094-166 | Mask Mother | unit | 3 | 3 | chaos |  |
| ven-095-166 | Shadow Order Disciple | unit | 2 | 2 | chaos | Burn |
| ven-096-166 | Shadowblade Lurker | unit | 5 | 5 | chaos |  |
| ven-097-166 | Spiderling | unit | 3 | 1 | chaos |  |
| ven-098-166 | Stargazer | unit | 5 | 4 | chaos | Flow |
| ven-099-166 | Tornado Warrior | unit (signature) | 3 | 3 | chaos | Empower |
| ven-100-166 | Up from the Deep | spell | 3 |  | chaos | Flow |
| ven-101-166 | Gust Monk | unit (signature) | 2 | 2 | chaos |  |
| ven-102-166 | Ravenbloom Prefect | unit | 3 | 3 | chaos |  |
| ven-103-166 | Shadows of the Past | spell | 3+1P |  | chaos |  |
| ven-104-166 | Tail-Cloaked Matriarch | unit | 4 | 4 | chaos | Empower |
| ven-105-166 | Twilight Step | spell | 2+1P |  | chaos | Flow |
| ven-106-166 | Wind and Ghosts | spell | 3+1P |  | chaos |  |
| ven-107-166 | Decree of Discord | spell | 1+1P |  | chaos |  |
| ven-108-166 | Forgotten Relic | gear | 5 |  | chaos | Burn |
| ven-109-166 | Illaoi, Prophet of the Great Kraken | unit (champion) | 6 | 4 | chaos |  |
| ven-110-166 | Mel, Defiant Soul | unit (champion) | 5 | 4 | chaos | Empower |
| ven-111-166 | Minah Swiftfoot | unit | 6+1P | 6 | chaos |  |
| ven-112-166 | Zed, Without a Sound | unit (champion) | 5 | 5 | chaos |  |
| ven-113-166 | Kennen, Storm of Shuriken | unit (champion) | 3+1P | 4 | chaos | Flow, Burn |
| ven-114-166 | Kharox | unit | 6 | 5 | chaos | Empower, Burn |
| ven-115-166 | Ocean Drake | unit | 8+2P | 7 | chaos |  |
| ven-116-166 | Dragon Form | spell | 3 |  | order | Flow |
| ven-117-166 | Disciple of Shen | unit | 2 | 1 | order |  |
| ven-118-166 | Horns of the Dragon | unit | 6 | 6 | order |  |
| ven-119-166 | Keeper of Law | unit (signature) | 5+1P | 5 | order |  |
| ven-120-166 | Masa, Crashing Thunder | unit (champion) | 4 | 4 | order |  |
| ven-121-166 | Reluctant Leader | unit | 4+1P | 3 | order |  |
| ven-122-166 | Solari Sunhawk | unit | 3 | 3 | order | Empower |
| ven-123-166 | Soulspinner | unit | 3 | 3 | order |  |
| ven-124-166 | Escaped Grayback | unit | 3 | 3 | order | Empower |
| ven-125-166 | Hungry Wolf | unit (signature) | 4 | 4 | order |  |
| ven-126-166 | Ki Barrier | spell | 2+1P |  | order |  |
| ven-127-166 | Lacerate | spell | 2+1P |  | order | Empower, Flow |
| ven-128-166 | Noxian Emissary | unit | 2 | 2 | order | Empower |
| ven-129-166 | Sacred Protector | unit | 4+1P | 6 | order |  |
| ven-130-166 | Aurok General | unit | 5 | 5 | order | Empower |
| ven-131-166 | Decree of Unity | spell | 2+1P |  | order |  |
| ven-132-166 | Fallen Feline | unit | 2+1P | 3 | order | Name |
| ven-133-166 | Glowstone | gear | 2 |  | order | Empower |
| ven-134-166 | Kayle, Justified | unit (champion) | 3 | 3 | order | Empower |
| ven-135-166 | Kennen, Keeper of Balance | unit (champion) | 3 | 2 | order |  |
| ven-136-166 | Ambessa, Respected and Feared | unit (champion) | 5 | 5 | order | Empower |
| ven-137-166 | Shady Spectacles | gear | 4 | 0 | order |  |
| ven-138-166 | Shen, Leader of the Kinkou Order | unit (champion) | 6+2P | 7 | order |  |
| ven-139-166 | Rogue Assassin | legend |  |  | fury/calm | Empower |
| ven-140-166 | Shuriken Flip | spell (signature) | 1+1P |  | fury/calm | Flow |
| ven-141-166 | Butcher of the Sands | legend |  |  | fury/body |  |
| ven-142-166 | Dominus | spell (signature) | 4 |  | fury/body |  |
| ven-143-166 | Master of Shadows | legend |  |  | fury/chaos | Empower |
| ven-144-166 | Death Mark | spell (signature) | 2+1P |  | fury/chaos | Flow, Burn |
| ven-145-166 | Curator of the Sands | legend |  |  | calm/mind |  |
| ven-146-166 | Siphoning Strike | spell (signature) | 4 |  | calm/mind |  |
| ven-147-166 | Eye of Twilight | legend |  |  | calm/order |  |
| ven-148-166 | Shadow Dash | spell (signature) | 2+1P |  | calm/order | Flow |
| ven-149-166 | Defender of Tomorrow | legend |  |  | mind/body | Empower |
| ven-150-166 | Acceleration Gate | spell (signature) | 3+1P |  | mind/body |  |
| ven-151-166 | Soul's Reflection | legend |  |  | mind/chaos | Empower |
| ven-152-166 | Rebuttal | spell (signature) | 1+1P |  | mind/chaos | New choices |
| ven-153-166 | Matriarch of War | legend |  |  | body/order | Empower |
| ven-154-166 | Public Execution | spell (signature) | 2+1P |  | body/order | Flow |
| ven-155-166 | Heart of the Tempest | legend |  |  | order/chaos | Empower |
| ven-156-166 | Lightning Rush | spell (signature) | 1 |  | order/chaos | Flow |
| ven-157-166 | Dragon Roost | battlefield |  |  |  |  |
| ven-158-166 | Heisho, Shell of the World | battlefield |  |  |  | Ignore |
| ven-159-166 | Kinkou Temple | battlefield |  |  |  |  |
| ven-160-166 | Mystic Vortex | battlefield |  |  |  |  |
| ven-161-166 | Piltovan Forge | battlefield |  |  |  |  |
| ven-162-166 | Protective Sands | battlefield |  |  |  |  |
| ven-163-166 | Risen Altar | battlefield |  |  |  | Empower |
| ven-164-166 | Sandswept Tomb | battlefield |  |  |  |  |
| ven-165-166 | Shadow Temple | battlefield |  |  |  | Burn |
| ven-166-166 | Threshold of the Gray | battlefield |  |  |  |  |

## Radiance — new cards

| Gallery id | Name | Type | Cost | Might | Domains | New mechanics |
|---|---|---|---|---|---|---|
| rad-001-167 | Amateur Demolitionist | unit | 2 | 1 | chaos | Deploy |
| rad-002-167 | Banner of Conquest | gear | 1+1P |  | fury | Deploy |
| rad-003-167 | Bilgewater Tracker | unit | 3 | 3 | body |  |
| rad-004-167 | Brawl | spell | 3+1P |  | chaos |  |
| rad-005-167 | Lingering Animus | unit | 2 | 2 | fury |  |
| rad-006-167 | Scorched Earth | spell | 4+1P |  |  |  |
| rad-007-167 | Rally for War | spell | 3+1P |  | fury |  |
| rad-008-167 | War Scout | unit | 3 | 2 | fury |  |
| rad-009-167 | Antagonistic Apparatus | unit | 6+1P | 3 |  |  |
| rad-012-167 | Runeblade Anvil | gear | 1 |  | chaos |  |
| rad-013-167 | Lost to the Sands | spell | 3+1P |  | fury |  |
| rad-014-167 | Shimmering Swimmer | unit | 3 | 3 | fury |  |
| rad-015-167 | Akali, Brash | unit (champion) | 5+1P | 5 | fury |  |
| rad-016-167 | Coalesced Energy | unit | 4+2P | 4 | mind |  |
| rad-017-167 | Detonation Amplifier | gear | 4+1P |  | chaos | Deploy |
| rad-018-167 | Drakehound Packmaster | unit | 9+2P | 9 | fury |  |
| rad-019-167 | Firebrand Raider | unit | 6+1P | 6 | fury |  |
| rad-020-167 | Mordekaiser, Twice Slain | unit (champion) | 7+1P | 5 | fury |  |
| rad-021-167 | Ekko, Revolutionary | unit (champion) | 5+1P | 4 | fury |  |
| rad-023-167 | Ziggs, Short-Fused | unit (champion) | 5+1P | 5 | fury | Deploy |
| rad-024-167 | Blossoming Dreamroot | gear | 2 |  | chaos | Deploy |
| rad-025-167 | Consult the Heavens | spell | 1 |  | calm |  |
| rad-026-167 | Entrenched Boar | unit | 2 | 2 |  |  |
| rad-027-167 | Iridescent Avian | unit | 3 | 3 | calm |  |
| rad-028-167 | Ki Burst | spell | 1 |  | calm |  |
| rad-029-167 | Life from Stone | spell | 1 |  | calm |  |
| rad-030-167 | Sanctum Conservator | unit | 5 | 5 | calm |  |
| rad-033-167 | Command: Attack | spell | 1+1P |  | calm | Deploy |
| rad-034-167 | Hunker Down | spell | 1 |  | calm |  |
| rad-035-167 | Invigorating Bloom | gear | 3 |  | calm |  |
| rad-036-167 | Storm Raptor | unit | 6 | 6 | calm |  |
| rad-037-167 | Watchful Curator | unit | 3+1P | 4 | calm |  |
| rad-038-167 | Ahri, Confident | unit (champion) | 4+1P | 4 | calm | Disarm |
| rad-039-167 | Ephemeral Ward | gear | 4+2P |  | calm |  |
| rad-040-167 | Heartening Soul | unit | 7+1P | 7 | calm |  |
| rad-041-167 | Hush | spell | 1+2P |  |  |  |
| rad-043-167 | Riven, Broken Blade | unit (champion) | 4 | 4 | calm |  |
| rad-045-167 | K'Sante, Dauntless | unit (champion) | 4+1P | 4 | calm |  |
| rad-046-167 | Resplendent Rostrum | gear | 8+1P |  | calm |  |
| rad-047-167 | Fanfare | spell | 3 |  |  |  |
| rad-048-167 | High Note | spell | 3 |  | mind |  |
| rad-049-167 | Industrious Mechsmith | unit | 4 | 1 |  |  |
| rad-050-167 | Mysterious Chronomancer | unit | 5 | 4 | mind | Disarm |
| rad-051-167 | Parallel Convergence | spell | 2+1P |  | mind |  |
| rad-052-167 | Resource Extractor | gear | 1 |  | mind | Deploy |
| rad-055-167 | Clanging Sentry | unit | 3 | 2 | mind | Disarm |
| rad-056-167 | Improvised Projectile | spell | 3+1P |  | mind |  |
| rad-057-167 | Piltovan Guidebot | unit | 4 | 3 | mind |  |
| rad-058-167 | Riptide Mage | unit | 4+1P | 3 | mind |  |
| rad-059-167 | Supernova | spell | 9+1P |  | order |  |
| rad-060-167 | The Empyrean | unit | 8 | 8 | mind | Show Off, Pick |
| rad-061-167 | Ekko, Ingenious | unit (champion) | 3 | 4 | mind |  |
| rad-062-167 | Gravity Field | gear | 5+1P |  | calm | Deploy |
| rad-063-167 | Kai'Sa, Rebel | unit (champion) | 5+1P | 5 | mind | Disarm |
| rad-064-167 | Scrutinizing Scope | gear | 2 |  | mind |  |
| rad-067-167 | Clocktower Guardian | unit | 8+2P | 8 | fury |  |
| rad-068-167 | Orianna, Mechanical Marvel | unit (champion) | 5 | 5 | mind |  |
| rad-069-167 | Zero Point Inflection | gear | 9+2P |  | mind |  |
| rad-070-167 | Hold 'Em | spell | 3 |  | body |  |
| rad-071-167 | Nazuman Ntofo | gear | 1 | 0 | order |  |
| rad-072-167 | Overloaded Traveler | unit | 2 | 2 | body | Deploy |
| rad-073-167 | Pillaged Armory | gear | 1 |  | calm | Deploy |
| rad-074-167 | Raving Witness | unit | 3 | 3 | body |  |
| rad-075-167 | Skystrike Squadron | unit | 5 | 4 | body |  |
| rad-076-167 | Sneaky Zeebles | unit | 2 | 1 | body |  |
| rad-077-167 | Wrath of the Freljord | unit | 9 | 7 | body |  |
| rad-079-167 | Avalanche | spell | 5+2P |  | body |  |
| rad-081-167 | Elnuk Herd | unit | 6 | 5 | fury |  |
| rad-082-167 | Primordial Roar | spell | 4+2P |  | body | Show Off, Pick |
| rad-083-167 | Stalwart Goatherd | unit | 4 | 2 | body |  |
| rad-084-167 | Cataclysm | spell | 4+1P |  | body |  |
| rad-085-167 | Graves, Blasting Through | unit (champion) | 5+1P | 4 | body |  |
| rad-086-167 | K'Sante, Courageous | unit (champion) | 5 | 4 | body |  |
| rad-087-167 | Portable Portal | gear | 2 |  | body | Deploy |
| rad-088-167 | Staunch Survivor | unit | 2+1P | 2 | body |  |
| rad-089-167 | Titanic Shrine | gear | 3 |  | body |  |
| rad-090-167 | Evelynn, Consuming | unit (champion) | 5+1P | 5 | body |  |
| rad-091-167 | Jarvan IV, Martial Paragon | unit (champion) | 4+1P | 4 | body | Deploy |
| rad-093-167 | Disposal Expert | unit | 4 | 3 | fury | Disarm |
| rad-095-167 | Momentary Companion | unit | 4 | 4 | chaos |  |
| rad-096-167 | Satchel Charge | spell | 3+1P |  | chaos | Deploy |
| rad-097-167 | Siren of the Shallows | unit | 2 | 1 | chaos | Disarm |
| rad-098-167 | Siren's Song | spell | 2 |  | chaos |  |
| rad-099-167 | Suave Negotiator | unit | 3 | 3 | chaos |  |
| rad-100-167 | The Waters' Wage | spell | 1 |  | chaos |  |
| rad-101-167 | An Offer You Can't Refuse | spell | 2 |  | chaos |  |
| rad-102-167 | Demon Shade | spell | 3 |  | fury |  |
| rad-105-167 | Reckless Innovator | unit | 3 | 3 | chaos |  |
| rad-106-167 | The Good Stuff | gear | 3+1P |  | body | Deploy |
| rad-107-167 | Bedeviling Stranger | unit | 2 | 2 | chaos |  |
| rad-108-167 | Bushwhack Trap | gear | 1 |  | fury | Deploy |
| rad-109-167 | Evelynn, In Control | unit (champion) | 4+1P | 4 | chaos | Disarm |
| rad-110-167 | Lulu, Whimsical | unit (champion) | 3 | 3 | chaos |  |
| rad-111-167 | Mechanical Bandit | gear | 4+1P |  | fury |  |
| rad-112-167 | Ziggs, Explosive | unit (champion) | 4 | 4 | chaos | Deploy |
| rad-114-167 | Riven, Deserter | unit (champion) | 5+1P | 5 | chaos |  |
| rad-115-167 | Transpositionist | unit | 6 | 4 | chaos |  |
| rad-116-167 | Concert Poster | gear | 1 |  | order | Deploy |
| rad-117-167 | For Demacia! | spell | 3 |  | order |  |
| rad-118-167 | Herald of the Emperor | unit | 5 | 3 | order |  |
| rad-119-167 | Hunting Dog | unit | 3 | 3 | order |  |
| rad-120-167 | Meticulous Restorationist | unit | 2 | 2 | order |  |
| rad-121-167 | One-Two Punch | spell | 3 |  |  |  |
| rad-122-167 | Party Crasher | unit | 9+1P | 6 | order |  |
| rad-125-167 | Dune Devourer | unit | 7+1P | 7 | order |  |
| rad-126-167 | Mournful Spirit | unit | 4 | 1 | order |  |
| rad-127-167 | Rousing Display | spell | 5 |  | order | Show Off, Pick |
| rad-128-167 | Steadfast Outrider | unit | 4 | 3 | order |  |
| rad-129-167 | Trifarian Captain | unit | 2 | 1 | order |  |
| rad-130-167 | Battlefield Armorer | unit | 3 | 3 | order |  |
| rad-131-167 | Demacian Illuminators | unit | 3+1P | 3 | order |  |
| rad-132-167 | Heimerdinger, Academy Instructor | unit (champion) | 6+1P | 5 | order |  |
| rad-133-167 | Jarvan IV, Royal Heir | unit (champion) | 7+1P | 7 | order |  |
| rad-136-167 | Dockside Lock-Up | gear | 6+2P |  | order | Deploy |
| rad-137-167 | Mordekaiser, Dominating | unit (champion) | 9+2P | 8 | order |  |
| rad-138-167 | Seraphine, Not Alone | unit (champion) | 5+1P | 1 | order |  |
| rad-139-167 | Boy Who Shattered Time | legend |  |  | fury/mind | Empower |
| rad-140-167 | Chronobreak | spell (signature) | 2 |  | fury/mind |  |
| rad-141-167 | Hexplosives Expert | legend |  |  | fury/chaos | Deploy |
| rad-142-167 | Hexplosive Minefield | spell (signature) | 2+1P |  | fury/chaos |  |
| rad-143-167 | Iron Revenant | legend |  |  | fury/order |  |
| rad-144-167 | Realm of Death | spell (signature) | 2+1P |  | fury/order |  |
| rad-145-167 | Lady of Clockwork | legend |  |  | calm/mind | Becomes unit |
| rad-146-167 | The Ball | gear (signature) | 5+1P |  | calm/mind | Deploy |
| rad-147-167 | Pride of Nazumah | legend |  |  | calm/body |  |
| rad-148-167 | Ntofo Strikes | spell (signature) | 2+1P |  | calm/body |  |
| rad-150-167 | Blade of the Exile | gear (signature) | 3+1P | 3 | calm/chaos |  |
| rad-151-167 | Starry-Eyed Songstress | legend |  |  | mind/order |  |
| rad-152-167 | Encore | spell (signature) | 3+1P |  | mind/order |  |
| rad-153-167 | Agony's Embrace | legend |  |  | body/chaos |  |
| rad-154-167 | Last Caress | spell (signature) | 2 |  | body/chaos |  |
| rad-155-167 | Exemplar of Demacia | legend |  |  | body/order | Deploy |
| rad-156-167 | Demacian Standard | gear (signature) | 2+1P |  | body/order | Deploy |
| rad-157-167 | Bandle Scouts' Academy | battlefield |  |  |  |  |
| rad-158-167 | Black Market | battlefield |  |  |  |  |
| rad-159-167 | Black Rose Sanctum | battlefield |  |  |  |  |
| rad-160-167 | Cosmic Vista | battlefield |  |  |  |  |
| rad-161-167 | Durand Memorial | battlefield |  |  |  |  |
| rad-162-167 | Firelights' Hideout | battlefield |  |  |  |  |
| rad-163-167 | Hunters' Circle | battlefield |  |  |  |  |
| rad-164-167 | Packed Amphitheater | battlefield |  |  |  |  |
| rad-165-167 | Rakelstake | battlefield |  |  |  |  |
| rad-166-167 | Ruined Monastery | battlefield |  |  |  |  |
| rad-167-167 | Neeko, Blending In | unit (champion) | 6+1P | 4 |  | Legend Zone |
