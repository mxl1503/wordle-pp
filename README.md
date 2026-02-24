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

## Project Structure

- `cpp-logic/`: C++ engine, WASM exports, and C++ tests.
- `frontend/`: Static web app (`index.html`, `styles.css`, `game.js`) and generated WASM bundle.
- `cpp-logic/data/wordlists/`: Answer/guess dictionaries and generated wordlist assets.
- `cpp-logic/data/data-utils/`: Utility scripts used to generate/sort wordlist data.

```text
wordle-pp/
|-- cpp-logic/
|   |-- include/wordle/
|   |   `-- engine.hpp
|   |-- src/
|   |   |-- engine.cpp
|   |   `-- wasm_exports.cpp
|   |-- tests/
|   |   |-- CMakeLists.txt
|   |   `-- engine_test.cpp
|   |-- data/wordlists/
|   |   |-- answerList.txt
|   |   |-- guessList.txt
|   |   |-- unsortedWordleAnswers.txt
|   |   `-- stringPermutations.txt
|   `-- data/data-utils/
|       `-- wordlist_tool.py
|-- frontend/
|   |-- index.html
|   |-- styles.css
|   |-- game.js
|   |-- icons/
|   |-- wordle.js
|   |-- wordle.wasm
|   `-- wordle.data
|-- Makefile
|-- CMakeLists.txt
|-- Dockerfile.build
|-- Dockerfile.test
|-- docker-compose.yml
|-- .github/workflows/ci.yml
`-- README.md
```

## Getting Started

### Dependencies

- Docker (required)
- Docker Compose plugin (usually included with Docker Desktop)

No local Emscripten installation is required.

### Docker Daemon Requirement

If you see `Cannot connect to the Docker daemon` when running `make build`, start Docker Desktop first, then verify Docker is reachable:

```sh
open -a Docker
docker version
```

After `docker version` succeeds, rerun:

```sh
make build
```

### Build with Docker

Compile `cpp-logic/src/wasm_exports.cpp` and `cpp-logic/src/engine.cpp` to `frontend/wordle.js`, `frontend/wordle.wasm`, and `frontend/wordle.data`:

```sh
make build
```

Equivalent Compose command:

```sh
docker compose run --rm build
```

### Serve Locally with Docker

Start a static web server on port `8080`:

```sh
make serve
```

Then open:

- [http://localhost:8080](http://localhost:8080)

Note: `make serve` serves the `frontend/` directory via nginx.

### Clean Build Artifacts

```sh
make clean
```

## Manual compile command (reference)

```sh
em++ -std=c++17 -O3 -Icpp-logic/include --preload-file cpp-logic/data/wordlists/answerList.txt --preload-file cpp-logic/data/wordlists/guessList.txt cpp-logic/src/wasm_exports.cpp cpp-logic/src/engine.cpp -o frontend/wordle.js -s WASM=1 \
-s EXPORTED_FUNCTIONS='["_createNewGame", "_makeGuess", "_validateGuess", "_makeHardModeGuess", "_freeCString", "_free", "_malloc"]' \
-s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap", "UTF8ToString", "stringToUTF8", "setValue"]'
```

### Run C++ Unit Tests (GoogleTest)

Install GoogleTest first (example on macOS):

```sh
brew install googletest
```

```sh
make test-cpp
```

If CMake cannot find GTest, install it and re-run `make test-cpp`.

Run the same C++ tests in Docker (no host GTest required):

```sh
make test-cpp-docker
```

### Formatting and Pre-Commit Hook

This repo uses [`.clang-format`](./.clang-format) for C/C++ formatting.

Install the local git pre-commit hook:

```sh
make install-hooks
```

The hook formats staged C/C++ files with `clang-format` and re-stages them before commit.

Check formatting locally:

```sh
make check-format-cpp
```

### Wordlist Utilities

Wordlist utility CLI docs are in:

- [`cpp-logic/data/data-utils/README.md`](./cpp-logic/data/data-utils/README.md)

## Impossible/Hard Mode

The Impossible/Hard mode dynamically adjusts the difficulty of the game. In this mode, the game does not select a single word at the start. Instead, it evaluates the player's guesses against all possible words and provides feedback that is the least informative, making it more challenging to guess the correct word. This mode is designed for players seeking an extra challenge beyond the traditional Wordle gameplay, and while still possible to beat, requires optimal guesses.

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
- **Wordle** (https://www.nytimes.com/games/wordle/index.html)
- **Solving Wordle using information theory** by 3B1B (https://www.youtube.com/watch?v=v68zYyaEmEA)
- **Optimal Wordle Solutions** by Jonathon Olsen (https://jonathanolson.net/experiments/optimal-wordle-solutions)
- **Wordle-solving state of the art: all optimality results so far** by Laurent Poirrier (https://www.poirrier.ca/notes/wordle-optimal/)
