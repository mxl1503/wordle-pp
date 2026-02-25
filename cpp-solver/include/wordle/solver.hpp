#ifndef WORDLE_SOLVER_HPP
#define WORDLE_SOLVER_HPP

#include <cstddef>
#include <string>

#include "wordle/engine.hpp"

namespace wordle {

	struct SolverRecommendation {
		std::string bestGuess;
		std::size_t remainingSolutions;
	};

	SolverRecommendation RecommendNextGuess(const WordList& answerList,
	                                        const WordList& guessList,
	                                        const WordList& prevGuesses,
	                                        const WordList& prevFeedback);

	SolverRecommendation RecommendNextGuess(const WordList& prevGuesses, const WordList& prevFeedback);

} // namespace wordle

#endif // WORDLE_SOLVER_HPP
