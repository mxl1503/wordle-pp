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

const char *createNewGame(void) {
    srand(time(NULL));
    std::vector<std::string> answerList = getAnswerList();
    int index = rand() % answerList.size();

    char *targetWord = new char[answerList[index].length() + 1];
    strcpy(targetWord, answerList[index].c_str());

    return targetWord;
}

std::string evaluateGuess(const std::string &guess, const std::string &targetWord) {
    std::string result(guess.length(), 'B');
    std::unordered_map<char, int> letterCount;

    for (char letter : targetWord) {
        letterCount[letter]++;
    }

    for (int i = 0; i < guess.length(); ++i) {
        if (guess[i] == targetWord[i]) {
            result[i] = 'G';
            letterCount[guess[i]]--;
        }
    }

    for (int i = 0; i < guess.length(); ++i) {
        if (guess[i] != targetWord[i] && letterCount[guess[i]] > 0) {
            result[i] = 'Y';
            letterCount[guess[i]]--;
        }
    }

    return result;
}

const char* makeGuess(const char* guess, const char *targetWord) {
    std::string feedback = evaluateGuess(std::string(guess), std::string(targetWord));
    char* result = new char[feedback.length() + 1];
    strcpy(result, feedback.c_str());

    return result;
}

bool validateGuess(const char* guess) {
    std::vector<std::string> guessList = getGuessList();
    return !!std::binary_search(guessList.begin(), guessList.end(), guess);
}

bool isMatch(const std::string& word, const std::string& guess, const std::string& feedback) {
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

std::vector<std::string> filterAnswerList(std::vector<std::string> prevGuesses, std::vector<std::string> prevFeedback) {
    std::vector<std::string> answerList = getAnswerList();
    if (prevGuesses.empty() && prevFeedback.empty()) {
        return answerList;
    }

    std::vector<std::string> filteredList;
    for (const auto &word : answerList) {
        bool matchesAll = true;
        for (int i = 0; i < prevGuesses.size(); ++i) {
            if (!isMatch(word, prevGuesses[i], prevFeedback[i])) {
                matchesAll = false;
                break;
            }
        }

        if (matchesAll) {
            filteredList.push_back(word);
        }
    }

    for (int i = 0; i < filteredList.size(); ++i) {
        std::cout << filteredList[i] << std::endl;
    }
    
    std::cout << filteredList.size() << std::endl;
    return filteredList;
}

std::string findWorstFeedback(std::string &guess, std::vector<std::string> &refinedAnswerList) {
    std::unordered_map<std::string, int> feedbackCount;

    // Count the frequency of each feedback
    for (const auto &word : refinedAnswerList) {
        std::string feedback = evaluateGuess(guess, word);
        feedbackCount[feedback]++;
    }

    // Find the feedback that is most common (least informative)
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

int main() {
    const char *prevGuessesArray[] = { "salet", "courd" };
    const char *prevFeedbackArray[] = { "BBBBB", "BYBYB" };
    const char *currentGuess = "gimpy";
    int arrayLength = 1;

    const char *feedback = makeHardModeGuess(prevGuessesArray, prevFeedbackArray, currentGuess, arrayLength);

    std::cout << "Feedback for guess '" << currentGuess << "': " << feedback << std::endl;

    return 0;
}