# Wordle-PP

Wordle-PP is an enhanced version of the popular word guessing game Wordle, implemented in C++ and compiled to WebAssembly using Emscripten for web browser compatibility.

## Description

This project is a full-stack implementation of Wordle with an added Impossible/Hard mode (detailed later) for an elevated challenge. The backend is written in C++ for optimal performance and compiled into WebAssembly, allowing it to run efficiently in modern web browsers. The frontend combines HTML, CSS, and JavaScript to create an interactive and responsive user interface.

## Features

- Classic Wordle gameplay with an additional Impossible/Hard mode.
- Backend logic in C++ compiled to WebAssembly for high performance.
- Responsive and interactive web-based frontend.
- Cross-browser compatibility.
- Dynamic difficulty adjustment in Impossible/Hard mode, providing an adaptive challenge based on player guesses.

## Getting Started

### Dependencies

- Emscripten SDK for compiling C++ to WebAssembly.
- Any modern web browser for running the game.

### Installing

- Clone the repository to your local machine.
- Ensure Emscripten SDK is installed and properly set up.
- Compile the C++ code to WebAssembly using Emscripten.

```sh
em++ -std=c++17 -O3 --preload-file wordLists/answerlist.txt --preload-file wordLists/guesslist.txt wordle.cpp -o wordle.js -s WASM=1 \
-s EXPORTED_FUNCTIONS='["_getAnswerList", "_getGuessList", "_createNewGame", "_makeGuess", "_evaluateGuess", "_validateGuess", "_isMatch", "_filterAnswerList", "_findWorstFeedback", "_makeHardModeGuess", "_free", "_malloc"]' \
-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "stringToUTF8", "setValue"]'
```

- Host the output files (wordle.js, wordle.data, wordle.wasm, and associated HTML/CSS files) on a web server.

### Executing program

- Open the hosted webpage in a browser to start playing the game.
- Choose between the classic mode and the Impossible/Hard mode for varying levels of difficulty.

## Impossible/Hard Mode

The Impossible/Hard mode dynamically adjusts the difficulty of the game. In this mode, the game does not select a single word at the start. Instead, it evaluates the player's guesses against all possible words and provides feedback that is the least informative, making it more challenging to guess the correct word. This mode is designed for players seeking an extra challenge beyond the traditional Wordle gameplay.

### How the Algorithm Works:
- **Dynamic Word Selection**: Rather than sticking to one preselected word, the game evaluates each guess against a list of all possible words.
- **Feedback Calculation**:
  - **Filtering Possible Answers**: Using the `filterAnswerList` function, the game filters out words from the answer list that do not match the previous guesses and feedback. This is done by:
    - Iterating over each word in the answer list.
    - Comparing the word against each previous guess and its feedback.
    - Keeping only those words that align with all previous feedback.
  - **Determining Least Informative Feedback**: The game then uses the `findWorstFeedback` function to find the feedback that reveals the least information about the possible words. This involves:
    - Comparing the current guess against each word in the filtered answer list.
    - Counting the frequency of each possible feedback outcome.
    - Selecting the most common (least informative) feedback to return to the player.
- **Implementation Details**:
  - The `makeHardModeGuess` function in the C++ code orchestrates this process, taking in previous guesses and their feedback, along with the current guess.
  - The algorithm ensures that the player is consistently challenged, making the game engaging and unpredictable.

## Acknowledgements

Project inspired by the following sources:
- Worlde (https://www.nytimes.com/games/wordle/index.html)
- Solving Wordle using information theory by 3B1B (https://www.youtube.com/watch?v=v68zYyaEmEA)
- Optimal Wordle Solutions by Jonathon Olsen (https://jonathanolson.net/experiments/optimal-wordle-solutions)
- Wordle-solving state of the art: all optimality results so far by "aurent Poirrier (https://www.poirrier.ca/notes/wordle-optimal/)