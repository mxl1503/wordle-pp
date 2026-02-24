#include <cstring>
#include <vector>

#include <emscripten.h>

#include "wordle/engine.hpp"

namespace {

char* CopyCString(const std::string& value) {
    char* out = new char[value.size() + 1];
    std::strcpy(out, value.c_str());
    return out;
}

}  // namespace

extern "C" {

EMSCRIPTEN_KEEPALIVE
const char* createNewGame(void) {
    return CopyCString(wordle::CreateNewGameWord());
}

EMSCRIPTEN_KEEPALIVE
const char* makeGuess(const char* guess, const char* targetWord) {
    return CopyCString(wordle::EvaluateGuess(guess, targetWord));
}

EMSCRIPTEN_KEEPALIVE
bool validateGuess(const char* guess) {
    return wordle::ValidateGuess(guess);
}

EMSCRIPTEN_KEEPALIVE
const char* makeHardModeGuess(
    const char** prevGuessesArray,
    const char** prevFeedbackArray,
    const char* guess,
    int arrayLength
) {
    std::vector<std::string> prevGuesses;
    std::vector<std::string> prevFeedback;
    prevGuesses.reserve(arrayLength);
    prevFeedback.reserve(arrayLength);

    for (int i = 0; i < arrayLength; ++i) {
        prevGuesses.emplace_back(prevGuessesArray[i]);
        prevFeedback.emplace_back(prevFeedbackArray[i]);
    }

    return CopyCString(wordle::MakeHardModeGuess(prevGuesses, prevFeedback, guess));
}

EMSCRIPTEN_KEEPALIVE
void freeCString(const char* str) {
    delete[] str;
}

}  // extern "C"
