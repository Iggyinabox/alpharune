/// @file test_card_data.cpp
/// Data-integrity tests on the loaded card database:
///   - Banned cards are actually marked CardDef::banned (official Standard ban list,
///     Rules Hub "Constructed Format Legality", last updated 2026-09-18).
///     (The --wild flag lets you PLAY banned cards; this verifies they're still
///     flagged on the card object.)
///   - Errata'd cards carry their corrected ability_text (from apply_errata.py
///     → registry, and post-refactor the C++ card literals).

#include <gtest/gtest.h>
#include "core/card_db.h"
#include "cards/card_registry.h"

#include <cstdlib>
#include <filesystem>
#include <string>

namespace riftbound {
namespace {

class CardDataTest : public ::testing::Test {
protected:
    void SetUp() override {
        registry.loadAll();
        db.buildFromClasses(registry);
    }
    CardRegistry registry;
    CardDB db;
};

// ── Banned cards ──────────────────────────────────────────────────────────
TEST_F(CardDataTest, BannedCardsAreMarkedBanned) {
    // Every name on the Standard ban list must be flagged on its CardDef.
    const char* banned[] = {
        "Called Shot", "Draven, Vanquisher", "Fight or Flight",
        "Scrapheap", "The Dreaming Tree", "Obelisk of Power",
        "Reaver's Row",
        // 2026-07-24
        "Stealthy Pursuer", "The Arena's Greatest", "Aspirant's Climb",
        // 2026-09-18
        "Ekko, Recurrent", "Stacked Deck",
    };
    for (const char* name : banned) {
        const CardDef* c = db.findByName(name);
        ASSERT_NE(c, nullptr) << name << " not found in registry";
        EXPECT_TRUE(c->banned) << name << " should be marked banned";
    }
}

TEST_F(CardDataTest, NonBannedCardIsNotBanned) {
    const CardDef* c = db.findByName("Blazing Scorcher");
    ASSERT_NE(c, nullptr);
    EXPECT_FALSE(c->banned) << "a legal card must not be flagged banned";
}

TEST_F(CardDataTest, BannedCountMatchesList) {
    int n = 0;
    for (const auto& [id, def] : db.all()) if (def.banned) ++n;
    // Exactly the 12 Standard ban-list names: a card banned by mistake is as
    // wrong as a missed ban. (Master Yi, Wuju Bladesman is 2v2-only; this
    // engine is 1v1, so he stays legal.)
    EXPECT_EQ(n, 12) << "banned-card count must match the Standard ban list";
}

// ── Errata adherence ──────────────────────────────────────────────────────
// Each card's ability_text must contain the distinctive phrase introduced by
// its official errata (apply_errata.py). If the data ever regresses to the
// pre-errata text, these fail.
TEST_F(CardDataTest, CardsAdhereToErrata) {
    struct Case { const char* name; const char* phrase; };
    const Case cases[] = {
        {"Disintegrate",       "If this kills it"},
        {"Convergent Mutation","increase its Might to the Might of another friendly unit"},
        {"Dune Drake",         "if there is a ready enemy unit here"},
        {"Dazzling Aurora",    "until you reveal a unit"},
        {"Blind Fury",         "Choose one and banish it"},
    };
    for (const auto& c : cases) {
        const CardDef* def = db.findByName(c.name);
        ASSERT_NE(def, nullptr) << c.name << " not found";
        EXPECT_NE(def->ability_text.find(c.phrase), std::string::npos)
            << c.name << " is missing errata phrase: \"" << c.phrase << "\"\n"
            << "  actual: " << def->ability_text;
    }
}

}  // namespace
}  // namespace riftbound
