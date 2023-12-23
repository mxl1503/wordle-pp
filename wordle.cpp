#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <unordered_map>

#include <emscripten.h>

extern "C" {

EMSCRIPTEN_KEEPALIVE
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

EMSCRIPTEN_KEEPALIVE
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

EMSCRIPTEN_KEEPALIVE
const char *createNewGame(void) {
    srand(time(NULL));
    std::vector<std::string> answerList = getAnswerList();
    int index = rand() % answerList.size();

    char *targetWord = new char[answerList[index].length() + 1];
    strcpy(targetWord, answerList[index].c_str());

    return targetWord;
}

EMSCRIPTEN_KEEPALIVE
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

EMSCRIPTEN_KEEPALIVE
const char* makeGuess(const char* guess, const char *targetWord) {
    std::string feedback = evaluateGuess(std::string(guess), std::string(targetWord));
    char* result = new char[feedback.length() + 1];
    strcpy(result, feedback.c_str());

    return result;
}

EMSCRIPTEN_KEEPALIVE
bool validateGuess(const char* guess) {
    std::vector<std::string> guessList = getGuessList();
    return !!std::binary_search(guessList.begin(), guessList.end(), guess);
}
}

/*
hard mode planning

for a guess, calculate the string of GBY that eliminates
the least solution.
makeGuess -> for each combination of GBY 3^5 = 243 possibilities
for each possibility, calculate number of possible answers remaining
return string with worst possible answer

*/ 
