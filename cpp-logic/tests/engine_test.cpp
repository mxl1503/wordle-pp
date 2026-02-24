#include <gtest/gtest.h>

#include "wordle/engine.hpp"

TEST(EvaluateGuessTest, HandlesDuplicateLetters) {
    EXPECT_EQ(wordle::EvaluateGuess("allee", "eagle"), "YYBYG");
}

TEST(IsMatchTest, MatchesExpectedWord) {
    EXPECT_TRUE(wordle::IsMatch("eagle", "allee", "YYBYG"));
    EXPECT_FALSE(wordle::IsMatch("angle", "allee", "YYBYG"));
}

TEST(FilterAnswerListTest, AppliesAllPreviousGuesses) {
    const wordle::WordList answerList = {"eagle", "angle", "amble"};
    const wordle::WordList prevGuesses = {"allee"};
    const wordle::WordList prevFeedback = {"YYBYG"};

    const wordle::WordList filtered =
        wordle::FilterAnswerList(answerList, prevGuesses, prevFeedback);

    ASSERT_EQ(filtered.size(), 1U);
    EXPECT_EQ(filtered[0], "eagle");
}

TEST(HardModeTest, ChoosesMostFrequentFeedback) {
    const wordle::WordList answerList = {"abcde", "abfde", "abgde"};
    const wordle::WordList prevGuesses = {};
    const wordle::WordList prevFeedback = {};

    EXPECT_EQ(
        wordle::MakeHardModeGuess(answerList, prevGuesses, prevFeedback, "abzzz"),
        "GGBBB"
    );
}
