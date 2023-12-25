#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

std::vector<std::string> getAnswerList(void) {
    std::vector<std::string> answerList;
    std::ifstream answerFile("wordLists/answerlist.txt");

    std::string word;
    while (answerFile >> word) {
        answerList.push_back(word);
    }

    answerFile.close();
    return answerList;
}

// Retrieves a list of valid guesses from a file
std::vector<std::string> getGuessList(void) {
    std::vector<std::string> guessList;
    std::ifstream guessFile("wordLists/guesslist.txt");

    std::string word;
    while (guessFile >> word) {
        guessList.push_back(word);
    }

    guessFile.close();
    return guessList;
}

// Creates a new game by selecting a random word from the answer list
const char *createNewGame(void) {
    srand(time(NULL));
    std::vector<std::string> answerList = getAnswerList();
    int index = rand() % answerList.size();

    char *targetWord = new char[answerList[index].length() + 1];
    strcpy(targetWord, answerList[index].c_str());

    return targetWord;
}

// Evaluates a guess against the target word and returns feedback
std::string evaluateGuess(const std::string &guess, const std::string &targetWord) {
    std::string result(guess.length(), 'B');
    std::unordered_map<char, int> letterCount;

    // Counting the occurrence of each character in the target word
    for (char letter : targetWord) {
        letterCount[letter]++;
    }

    // First pass for greens (correct position)
    for (int i = 0; i < guess.length(); ++i) {
        if (guess[i] == targetWord[i]) {
            result[i] = 'G';
            letterCount[guess[i]]--;
        }
    }

    // Second pass for yellows (wrong position) and greys (not in word)
    for (int i = 0; i < guess.length(); ++i) {
        if (guess[i] != targetWord[i] && letterCount[guess[i]] > 0) {
            result[i] = 'Y';
            letterCount[guess[i]]--;
        }
    }

    return result;
}

// Wrapper function for evaluating a guess in C-style string
const char *makeGuess(const char *guess, const char *targetWord) {
    std::string feedback = evaluateGuess(std::string(guess), std::string(targetWord));
    char *result = new char[feedback.length() + 1];
    strcpy(result, feedback.c_str());

    return result;
}

// Validates if a guess is a valid word
bool validateGuess(const char *guess) {
    std::vector<std::string> guessList = getGuessList();
    return !!std::binary_search(guessList.begin(), guessList.end(), guess);
}

// Checks if a word matches given guess and feedback
bool isMatch(const std::string &word, const std::string &guess, const std::string &feedback) {
    std::unordered_map<char, int> charCount;
    for (char c : word) {
        charCount[c]++;
    }

    // Check for greens
    for (int i = 0; i < guess.size(); ++i) {
        if (feedback[i] == 'G') {
            if (word[i] != guess[i]) {
                return false;
            }
            charCount[guess[i]]--;
        }
    }

    // Check for yellows and greys
    for (int i = 0; i < guess.size(); ++i) {
        if (feedback[i] == 'Y') {
            if (word[i] == guess[i] || charCount[guess[i]] <= 0) {
                return false;
            }
            charCount[guess[i]]--;
        } else if (feedback[i] == 'B' && charCount[guess[i]] > 0 && word.find(guess[i]) != std::string::npos) {
            return false;
        }
    }

    return true;
}

// Filters the answer list based on previous guesses and feedback
std::vector<std::string> filterAnswerList(std::vector<std::string> prevGuesses, std::vector<std::string> prevFeedback) {
    std::vector<std::string> answerList = getAnswerList();
    if (prevGuesses.empty() && prevFeedback.empty()) {
        return answerList;
    }

    std::vector<std::string> filteredList;
    for (const auto &word : answerList) {
        bool matchesAll = true;

        // Check current word against previous guesses and feedback
        for (int i = 0; i < prevGuesses.size(); ++i) {
            if (!isMatch(word, prevGuesses[i], prevFeedback[i])) {
                matchesAll = false;
                break;
            }
        }

        // If the current word matches past guesses and feedback, add it to the list
        if (matchesAll) {
            filteredList.push_back(word);
        }
    }

    return filteredList;
}

// Finds the most common (least informative) feedback based on the current guess and refined answer list
std::string findWorstFeedback(std::string &guess, std::vector<std::string> &refinedAnswerList) {
    std::unordered_map<std::string, int> feedbackCount;

    // Count the frequency of each feedback
    for (const auto &word : refinedAnswerList) {
        std::string feedback = evaluateGuess(guess, word);
        feedbackCount[feedback]++;
    }

    // Find the most common feedback
    std::string worstFeedback;
    int maxCount = 0;
    for (const auto &feedbackPair : feedbackCount) {
        if (feedbackPair.second > maxCount) {
            worstFeedback = feedbackPair.first;
            maxCount = feedbackPair.second;
        }
    }

    return worstFeedback;
}

// Wrapper function for the hard mode guessing logic
const char *makeHardModeGuess(const char **prevGuessesArray, const char **prevFeedbackArray, const char *guess, int arrayLength) {
    std::vector<std::string> prevGuesses;
    std::vector<std::string> prevFeedback;

    for (int i = 0; i < arrayLength; ++i) {
        prevGuesses.push_back(std::string(prevGuessesArray[i]));
        prevFeedback.push_back(std::string(prevFeedbackArray[i]));
    }

    std::string currentGuess = std::string(guess);
    std::vector<std::string> refinedAnswerList = filterAnswerList(prevGuesses, prevFeedback);

    std::string feedback = findWorstFeedback(currentGuess, refinedAnswerList);

    // Convert feedback to a C-style string for return
    char *feedbackCStr = new char[feedback.size() + 1];
    strcpy(feedbackCStr, feedback.c_str());
    return feedbackCStr;
}

int main(void) {
    const char *prevGuessesArray[] = { "salet", "courd" };
    const char *prevFeedbackArray[] = { "BBBBB", "BYBYB" };
    const char *currentGuess = "gimpy";
    int arrayLength = 2;

    const char *feedback = makeHardModeGuess(prevGuessesArray, prevFeedbackArray, currentGuess, arrayLength);

    std::cout << "Feedback for guess '" << currentGuess << "': " << feedback << std::endl;

    return 0;
}