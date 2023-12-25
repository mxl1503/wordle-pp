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

- Host the output files (wordle.js, wordle.wasm, and associated HTML/CSS files) on a web server.

### Executing program

- Open the hosted webpage in a browser to start playing the game.
- Choose between the classic mode and the Impossible/Hard mode for varying levels of difficulty.

## Impossible/Hard Mode

The Impossible/Hard mode dynamically adjusts the difficulty of the game. In this mode, the game does not select a single word at the start. Instead, it evaluates the player's guesses against all possible words and provides feedback that is the least informative, making it more challenging to guess the correct word. This mode is designed for players seeking an extra challenge beyond the traditional Wordle gameplay.
