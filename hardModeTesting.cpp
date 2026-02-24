#include <iostream>

#include "wordle/engine.hpp"

int main() {
    const wordle::WordList prevGuesses = {"salet", "courd"};
    const wordle::WordList prevFeedback = {"BBBBB", "BYBYB"};
    const std::string currentGuess = "gimpy";

    const std::string feedback =
        wordle::MakeHardModeGuess(prevGuesses, prevFeedback, currentGuess);

    std::cout << "Feedback for guess '" << currentGuess << "': " << feedback << '\n';
    return 0;
}
