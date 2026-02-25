#include <gtest/gtest.h>

#include "wordle/engine.hpp"
#include "wordle/solver.hpp"

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

	const wordle::WordList filtered = wordle::FilterAnswerList(answerList, prevGuesses, prevFeedback);

	ASSERT_EQ(filtered.size(), 1U);
	EXPECT_EQ(filtered[0], "eagle");
}

TEST(HardModeTest, ChoosesMostFrequentFeedback) {
	const wordle::WordList answerList = {"abcde", "abfde", "abgde"};
	const wordle::WordList prevGuesses = {};
	const wordle::WordList prevFeedback = {};

	EXPECT_EQ(wordle::MakeHardModeGuess(answerList, prevGuesses, prevFeedback, "abzzz"), "GGBBB");
}

TEST(SolverTest, ReturnsBestGuessAndRemainingCount) {
	const wordle::WordList answerList = {"cigar", "rebut", "sissy"};
	const wordle::WordList guessList = {"cigar", "rebut", "sissy", "tares"};
	const wordle::WordList prevGuesses = {"cigar"};
	const wordle::WordList prevFeedback = {wordle::EvaluateGuess("cigar", "rebut")};

	const wordle::WordList filtered = wordle::FilterAnswerList(answerList, prevGuesses, prevFeedback);
	const wordle::SolverRecommendation recommendation =
	    wordle::RecommendNextGuess(answerList, guessList, prevGuesses, prevFeedback);

	EXPECT_FALSE(recommendation.bestGuess.empty());
	EXPECT_EQ(recommendation.remainingSolutions, filtered.size());
}
