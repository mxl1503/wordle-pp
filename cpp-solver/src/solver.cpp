#include "wordle/solver.hpp"

#include <unordered_map>
#include <unordered_set>
#include <limits>

namespace wordle {
	namespace {

		double ExpectedRemaining(std::string_view guess, const WordList& possibleSolutions) {
			if (possibleSolutions.empty()) {
				return 0.0;
			}

			std::unordered_map<std::string, std::size_t> buckets;
			buckets.reserve(possibleSolutions.size());
			for (const auto& solution : possibleSolutions) {
				++buckets[EvaluateGuess(guess, solution)];
			}

			const double total = static_cast<double>(possibleSolutions.size());
			double sum = 0.0;
			for (const auto& [_, count] : buckets) {
				const double c = static_cast<double>(count);
				sum += c * c;
			}
			return sum / total;
		}

	} // namespace

	SolverRecommendation RecommendNextGuess(const WordList& prevGuesses, const WordList& prevFeedback) {
		if (prevGuesses.empty() && prevFeedback.empty()) {
			return {"roate", 2315};
		}
		return RecommendNextGuess(GetAnswerList(), GetGuessList(), prevGuesses, prevFeedback);
	}

	SolverRecommendation RecommendNextGuess(const WordList& answerList,
	                                        const WordList& guessList,
	                                        const WordList& prevGuesses,
	                                        const WordList& prevFeedback) {
		const WordList possibleSolutions = FilterAnswerList(answerList, prevGuesses, prevFeedback);
		if (possibleSolutions.empty()) {
			return {"", 0};
		}

		std::unordered_set<std::string> solutionSet(possibleSolutions.begin(), possibleSolutions.end());

		std::string bestGuess = possibleSolutions.front();
		double bestScore = std::numeric_limits<double>::infinity();
		bool bestGuessIsSolution = false;

		for (const auto& guess : guessList) {
			const double score = ExpectedRemaining(guess, possibleSolutions);
			const bool guessIsSolution = solutionSet.find(guess) != solutionSet.end();
			const bool betterScore = score < bestScore;
			const bool tieBreakBySolution = score == bestScore && guessIsSolution && !bestGuessIsSolution;
			const bool tieBreakLexical = score == bestScore && guessIsSolution == bestGuessIsSolution
			                             && guess < bestGuess;

			if (betterScore || tieBreakBySolution || tieBreakLexical) {
				bestScore = score;
				bestGuess = guess;
				bestGuessIsSolution = guessIsSolution;
			}
		}

		return {bestGuess, possibleSolutions.size()};
	}

} // namespace wordle
