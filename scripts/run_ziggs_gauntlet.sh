#!/usr/bin/env bash
# Round robin for the three Ziggs lists against each other and the meta decks.
# Each pairing plays GAMES games, half with each deck in the first seat, with
# the same agent in both seats. Results append to OUT as CSV:
#   deck_a,deck_b,a_wins,b_wins,draws,games,agent
set -euo pipefail
cd "$(dirname "$0")/.."

AGENT="${AGENT:-ismcts:sims=50}"
GAMES="${GAMES:-20}"
THREADS="${THREADS:-2}"
OUT="${OUT:-results/ziggs_gauntlet.csv}"
SEED_BASE="${SEED_BASE:-1000}"
mkdir -p "$(dirname "$OUT")"
[ -f "$OUT" ] || echo "deck_a,deck_b,a_wins,b_wins,draws,games,agent" > "$OUT"


META=(meta_kennen_la meta_akali_la meta_kaisa_vancouver)

play() {  # play <p1> <p2> <games> <seed> -> prints "p1wins p2wins draws"
  ./build/riftbound --agent1 "$AGENT" --agent2 "$AGENT" \
      --deck1 "decks/$1.txt" --deck2 "decks/$2.txt" \
      --games "$3" --threads "$THREADS" --seed "$4" --web off 2>&1 |
    awk '/P1 wins:/{a=$3} /P2 wins:/{b=$3} /Draws:/{d=$2}
         /^Game over: winner=1/{a++} /^Game over: winner=2/{b++}
         /^Game over: winner=0/{d++}
         END{print a+0, b+0, d+0}'
}

pairing() {  # pairing <a> <b>
  local a="$1" b="$2" half=$((GAMES / 2)) seed=$((SEED_BASE + RANDOM))
  read -r a1 b1 d1 < <(play "$a" "$b" "$half" "$seed")
  read -r b2 a2 d2 < <(play "$b" "$a" "$half" "$((seed + 500))")
  echo "$a,$b,$((a1 + a2)),$((b1 + b2)),$((d1 + d2)),$GAMES,$AGENT" | tee -a "$OUT"
}

# STAGE splits the run so each part fits a 2-hour job:
#   1   = merged and original lists vs the meta decks
#   2   = control list vs the meta decks, plus the three Ziggs head-to-heads
#   all = everything (default)
# PAIRS="deckA:deckB deckC:deckD" runs just those pairings instead.
if [ -n "${PAIRS:-}" ]; then
  for p in $PAIRS; do pairing "${p%%:*}" "${p##*:}"; done
  exit 0
fi
STAGE="${STAGE:-all}"
if [ "$STAGE" = 1 ] || [ "$STAGE" = all ]; then
  for z in ziggs_merged ziggs_nacho_original; do
    for m in "${META[@]}"; do pairing "$z" "$m"; done
  done
fi
if [ "$STAGE" = 2 ] || [ "$STAGE" = all ]; then
  for m in "${META[@]}"; do pairing ziggs_claude_control "$m"; done
  pairing ziggs_merged ziggs_nacho_original
  pairing ziggs_merged ziggs_claude_control
  pairing ziggs_nacho_original ziggs_claude_control
fi
