#include "wordle/engine.hpp"

#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <random>
#include <stdexcept>

namespace wordle {
	namespace {

		WordList LoadWordList(const char* path) {
			std::ifstream file(path);
			if (!file.is_open()) {
				throw std::runtime_error(std::string("Failed to open word list: ") + path);
			}

			WordList words;
			std::string word;
			while (file >> word) {
				words.push_back(word);
			}
			return words;
		}

		std::mt19937& RNG() {
			static std::mt19937 rng(std::random_device{}());
			return rng;
		}

	} // namespace

	const WordList& GetAnswerList() {
		static const WordList answerList = LoadWordList("cpp-logic/data/wordlists/answerList.txt");
		return answerList;
	}

	const WordList& GetGuessList() {
		static const WordList guessList = LoadWordList("cpp-logic/data/wordlists/guessList.txt");
		return guessList;
	}

	std::string CreateNewGameWord() {
		const auto& answerList = GetAnswerList();
		std::uniform_int_distribution<std::size_t> dist(0, answerList.size() - 1);
		return answerList[dist(RNG())];
	}

	bool ValidateGuess(std::string_view guess) {
		const auto& guessList = GetGuessList();
		return std::binary_search(guessList.begin(), guessList.end(), std::string(guess));
	}

	std::string EvaluateGuess(std::string_view guess, std::string_view targetWord) {
		std::string result(guess.size(), 'B');
		std::unordered_map<char, int> letterCount;

		for (char letter : targetWord) {
			letterCount[letter]++;
		}

		for (std::size_t i = 0; i < guess.size(); ++i) {
			if (guess[i] == targetWord[i]) {
				result[i] = 'G';
				letterCount[guess[i]]--;
			}
		}

		for (std::size_t i = 0; i < guess.size(); ++i) {
			if (guess[i] != targetWord[i] && letterCount[guess[i]] > 0) {
				result[i] = 'Y';
				letterCount[guess[i]]--;
			}
		}

		return result;
	}

	bool IsMatch(std::string_view word, std::string_view guess, std::string_view feedback) {
		std::unordered_map<char, int> charCount;
		for (char c : word) {
			charCount[c]++;
		}

		for (std::size_t i = 0; i < guess.size(); ++i) {
			if (feedback[i] == 'G') {
				if (word[i] != guess[i]) {
					return false;
				}
				charCount[guess[i]]--;
			}
		}

		for (std::size_t i = 0; i < guess.size(); ++i) {
			if (feedback[i] == 'Y') {
				if (word[i] == guess[i] || charCount[guess[i]] <= 0) {
					return false;
				}
				charCount[guess[i]]--;
			}
			else if (feedback[i] == 'B' && charCount[guess[i]] > 0 && word.find(guess[i]) != std::string::npos) {
				return false;
			}
		}

		return true;
	}

	WordList FilterAnswerList(const WordList& answerList, const WordList& prevGuesses, const WordList& prevFeedback) {
		if (prevGuesses.empty() && prevFeedback.empty()) {
			return answerList;
		}

		WordList filteredList;
		filteredList.reserve(answerList.size());

		for (const auto& word : answerList) {
			bool matchesAll = true;
			for (std::size_t i = 0; i < prevGuesses.size(); ++i) {
				if (!IsMatch(word, prevGuesses[i], prevFeedback[i])) {
					matchesAll = false;
					break;
				}
			}
			if (matchesAll) {
				filteredList.push_back(word);
			}
		}

		return filteredList;
	}

	WordList FilterAnswerList(const WordList& prevGuesses, const WordList& prevFeedback) {
		return FilterAnswerList(GetAnswerList(), prevGuesses, prevFeedback);
	}

	std::string FindWorstFeedback(std::string_view guess, const WordList& refinedAnswerList) {
		std::unordered_map<std::string, int> feedbackCount;

		for (const auto& word : refinedAnswerList) {
			std::string feedback = EvaluateGuess(guess, word);
			feedbackCount[feedback]++;
		}

		std::string worstFeedback;
		int maxCount = 0;
		for (const auto& feedbackPair : feedbackCount) {
			if (feedbackPair.second > maxCount) {
				worstFeedback = feedbackPair.first;
				maxCount = feedbackPair.second;
			}
		}

		return worstFeedback;
	}

	std::string MakeHardModeGuess(const WordList& answerList,
	                              const WordList& prevGuesses,
	                              const WordList& prevFeedback,
	                              std::string_view guess) {
		WordList refinedAnswerList = FilterAnswerList(answerList, prevGuesses, prevFeedback);
		return FindWorstFeedback(guess, refinedAnswerList);
	}

	std::string MakeHardModeGuess(const WordList& prevGuesses, const WordList& prevFeedback, std::string_view guess) {
		return MakeHardModeGuess(GetAnswerList(), prevGuesses, prevFeedback, guess);
	}

} // namespace wordle
