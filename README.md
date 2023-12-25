# Wordle-PP

Wordle-PP is a Wordle-like word guessing game implemented in C++ and compiled to WebAssembly using Emscripten, allowing it to run in web browsers.

## Description

This project is a full-stack implementation of the popular word game Wordle. The backend logic is written in C++ for performance reasons and compiled into WebAssembly, enabling it to run directly in modern web browsers. The frontend is developed with HTML, CSS, and JavaScript, providing an interactive and responsive user interface.

## Features

- Word guessing game based on Wordle.
- Backend written in C++ and compiled to WebAssembly.
- Responsive web-based frontend.
- Supports various browsers.

## Getting Started

### Dependencies

- Emscripten SDK for compiling C++ to WebAssembly.
- Any modern web browser for running the game.

### Installing

- Clone the repository to your local machine.
- Ensure Emscripten SDK is installed and properly set up.
- Compile the C++ code to WebAssembly using Emscripten.

```sh
em++ -std=c++17 -O1 --preload-file wordLists/answerlist.txt --preload-file wordLists/guesslist.txt wordle.cpp -o wordle.js -s WASM=1 \
-s EXPORTED_FUNCTIONS='["_getAnswerList", "_getGuessList", "_createNewGame", "_makeGuess", "_evaluateGuess", "_validateGuess", "_isMatch", "_filterAnswerList", "_findWorstFeedback", "_makeHardModeGuess", "_free", "_malloc"]' \
-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "stringToUTF8", "setValue"]'

```
- Host the output files (wordle.js, wordle.wasm, and associated HTML/CSS files) on a web server.

### Executing program

- Open the hosted webpage in a browser to play the game.
