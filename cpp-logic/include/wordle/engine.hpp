#ifndef WORDLE_ENGINE_HPP
#define WORDLE_ENGINE_HPP

#include <string_view>
#include <string>
#include <vector>

namespace wordle {

	using WordList = std::vector<std::string>;

	const WordList& GetAnswerList();
	const WordList& GetGuessList();

	std::string CreateNewGameWord();
	bool ValidateGuess(std::string_view guess);

	std::string EvaluateGuess(std::string_view guess, std::string_view targetWord);
	bool IsMatch(std::string_view word, std::string_view guess, std::string_view feedback);

	WordList FilterAnswerList(const WordList& answerList, const WordList& prevGuesses, const WordList& prevFeedback);

	WordList FilterAnswerList(const WordList& prevGuesses, const WordList& prevFeedback);

	std::string FindWorstFeedback(std::string_view guess, const WordList& refinedAnswerList);

	std::string MakeHardModeGuess(const WordList& answerList,
	                              const WordList& prevGuesses,
	                              const WordList& prevFeedback,
	                              std::string_view guess);

	std::string MakeHardModeGuess(const WordList& prevGuesses, const WordList& prevFeedback, std::string_view guess);

} // namespace wordle

#endif // WORDLE_ENGINE_HPP
