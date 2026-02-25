(function () {
  const { useEffect, useMemo, useRef, useState } = React;

  const GRID_ROWS = 6;
  const GRID_COLS = 5;
  const KEYBOARD_ROWS = [
    ["Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"],
    ["A", "S", "D", "F", "G", "H", "J", "K", "L"],
    ["Enter", "Z", "X", "C", "V", "B", "N", "M", "Backspace"]
  ];

  function buildInitialBoard() {
    return Array.from({ length: GRID_ROWS }, (_, row) =>
      Array.from({ length: GRID_COLS }, (_, col) => ({
        id: "cell-" + row + "-" + col,
        letter: "",
        colour: "",
        flip: false
      }))
    );
  }

  function getColourFromFeedback(char) {
    switch (char) {
      case "G":
        return "rgb(106, 169, 100)";
      case "Y":
        return "yellow";
      default:
        return "rgb(120, 124, 126)";
    }
  }

  function getKeyboardState(feedbackRows, guessRows) {
    const rank = { absent: 1, present: 2, correct: 3 };
    const state = {};

    for (let row = 0; row < feedbackRows.length; row++) {
      const feedback = feedbackRows[row];
      const guess = guessRows[row];
      for (let col = 0; col < guess.length; col++) {
        const letter = guess[col].toUpperCase();
        let candidate = "absent";
        if (feedback[col] === "G") {
          candidate = "correct";
        } else if (feedback[col] === "Y") {
          candidate = "present";
        }

        if (!state[letter] || rank[candidate] > rank[state[letter]]) {
          state[letter] = candidate;
        }
      }
    }

    return state;
  }

  function WordleApp() {
    const [runtimeReady, setRuntimeReady] = useState(false);
    const [hardMode, setHardMode] = useState(false);
    const [board, setBoard] = useState(buildInitialBoard);
    const [currentWord, setCurrentWord] = useState("");
    const [wordsSubmitted, setWordsSubmitted] = useState(0);
    const [prevGuesses, setPrevGuesses] = useState([]);
    const [prevFeedback, setPrevFeedback] = useState([]);
    const [showVictory, setShowVictory] = useState(false);
    const [showDefeat, setShowDefeat] = useState(false);
    const [showInvalidWord, setShowInvalidWord] = useState(false);
    const targetWordPtrRef = useRef(0);
    const hideInvalidWordTimeoutRef = useRef(0);
    const runtimeInitDoneRef = useRef(false);

    const keyboardState = useMemo(
      () => getKeyboardState(prevFeedback, prevGuesses),
      [prevFeedback, prevGuesses]
    );

    useEffect(() => {
      function initRuntime() {
        if (runtimeInitDoneRef.current) {
          return;
        }
        runtimeInitDoneRef.current = true;
        if (targetWordPtrRef.current) {
          Module._freeCString(targetWordPtrRef.current);
        }
        targetWordPtrRef.current = Module._createNewGame();
        setRuntimeReady(true);
      }

      function canUseModuleExports() {
        return typeof Module !== "undefined" && typeof Module._createNewGame === "function";
      }

      if (canUseModuleExports()) {
        initRuntime();
      }

      const previousOnRuntimeInitialized = Module.onRuntimeInitialized;
      Module.onRuntimeInitialized = function () {
        if (typeof previousOnRuntimeInitialized === "function") {
          previousOnRuntimeInitialized();
        }
        initRuntime();
      };

      // Covers the race where runtime has already initialized before we set the callback.
      const initPollId = setInterval(function () {
        if (canUseModuleExports()) {
          initRuntime();
          clearInterval(initPollId);
        }
      }, 50);

      return function () {
        clearInterval(initPollId);
        if (targetWordPtrRef.current) {
          Module._freeCString(targetWordPtrRef.current);
          targetWordPtrRef.current = 0;
        }
        if (hideInvalidWordTimeoutRef.current) {
          clearTimeout(hideInvalidWordTimeoutRef.current);
        }
      };
    }, []);

    useEffect(() => {
      function onBeforeUnload() {
        if (targetWordPtrRef.current) {
          Module._freeCString(targetWordPtrRef.current);
          targetWordPtrRef.current = 0;
        }
      }

      window.addEventListener("beforeunload", onBeforeUnload);
      return function () {
        window.removeEventListener("beforeunload", onBeforeUnload);
      };
    }, []);

    useEffect(() => {
      if (!runtimeReady) {
        return;
      }

      function onKeyDown(event) {
        if (
          event.key === "Backspace" ||
          event.key === "Enter" ||
          /^[A-Z]$/i.test(event.key)
        ) {
          handleKey(event.key);
        }
      }

      document.addEventListener("keydown", onKeyDown);
      return function () {
        document.removeEventListener("keydown", onKeyDown);
      };
    }, [runtimeReady, showVictory, showDefeat, currentWord, prevGuesses, prevFeedback, hardMode, wordsSubmitted]);

    useEffect(() => {
      if (showInvalidWord) {
        if (hideInvalidWordTimeoutRef.current) {
          clearTimeout(hideInvalidWordTimeoutRef.current);
        }
        hideInvalidWordTimeoutRef.current = setTimeout(function () {
          setShowInvalidWord(false);
        }, 1000);
      }
    }, [showInvalidWord]);

    useEffect(() => {
      if (runtimeReady) {
        setCurrentWord("");
        resetGame();
      }
    }, [hardMode, runtimeReady]);

    function allocateStringArray(strings) {
      const ptrs = strings.map(function (str) {
        const buffer = Module._malloc(str.length + 1);
        Module.stringToUTF8(str, buffer, str.length + 1);
        return buffer;
      });

      const arrayPtr = Module._malloc(ptrs.length * 4);
      ptrs.forEach(function (ptr, index) {
        Module.setValue(arrayPtr + index * 4, ptr, "i32");
      });

      return [arrayPtr, ptrs];
    }

    function hardModeGuessHelper(guesses, feedbacks, currentWordCPP) {
      const [prevGuessesArrPtr, ptrsOne] = allocateStringArray(guesses);
      const [prevFeedbackArrPtr, ptrsTwo] = allocateStringArray(feedbacks);

      const returnVal = Module._makeHardModeGuess(
        prevGuessesArrPtr,
        prevFeedbackArrPtr,
        currentWordCPP,
        guesses.length
      );

      ptrsOne.forEach(function (ptr) {
        Module._free(ptr);
      });
      ptrsTwo.forEach(function (ptr) {
        Module._free(ptr);
      });
      Module._free(prevGuessesArrPtr);
      Module._free(prevFeedbackArrPtr);

      return returnVal;
    }

    function resetGame() {
      setShowVictory(false);
      setShowDefeat(false);
      setShowInvalidWord(false);
      setWordsSubmitted(0);
      setCurrentWord("");
      setPrevGuesses([]);
      setPrevFeedback([]);
      setBoard(buildInitialBoard());

      if (!runtimeReady) {
        return;
      }
      if (targetWordPtrRef.current) {
        Module._freeCString(targetWordPtrRef.current);
      }
      targetWordPtrRef.current = Module._createNewGame();
    }

    function updateBoardWithCurrentWord(word, rowIndex) {
      setBoard(function (prevBoard) {
        const nextBoard = prevBoard.map(function (row) {
          return row.map(function (cell) {
            return { ...cell };
          });
        });
        for (let col = 0; col < GRID_COLS; col++) {
          nextBoard[rowIndex][col].letter = word[col] || "";
        }
        return nextBoard;
      });
    }

    function applyFeedback(rowIndex, guess, feedback) {
      setBoard(function (prevBoard) {
        const nextBoard = prevBoard.map(function (row) {
          return row.map(function (cell) {
            return { ...cell };
          });
        });

        for (let col = 0; col < GRID_COLS; col++) {
          nextBoard[rowIndex][col].letter = guess[col].toUpperCase();
          nextBoard[rowIndex][col].colour = getColourFromFeedback(feedback[col]);
          nextBoard[rowIndex][col].flip = true;
        }

        return nextBoard;
      });

      setTimeout(function () {
        setBoard(function (prevBoard) {
          const nextBoard = prevBoard.map(function (row) {
            return row.map(function (cell) {
              return { ...cell };
            });
          });
          for (let col = 0; col < GRID_COLS; col++) {
            nextBoard[rowIndex][col].flip = false;
          }
          return nextBoard;
        });
      }, 800);
    }

    function submitGuess() {
      const guess = currentWord.toLowerCase();
      const currentWordCPP = Module._malloc(guess.length + 1);
      Module.stringToUTF8(guess, currentWordCPP, guess.length + 1);

      if (guess.length === GRID_COLS && Module._validateGuess(currentWordCPP)) {
        const feedbackPtr = hardMode
          ? hardModeGuessHelper(prevGuesses, prevFeedback, currentWordCPP)
          : Module._makeGuess(currentWordCPP, targetWordPtrRef.current);

        const feedback = Module.UTF8ToString(feedbackPtr);
        Module._freeCString(feedbackPtr);

        applyFeedback(wordsSubmitted, guess, feedback);
        const nextGuesses = prevGuesses.concat(guess);
        const nextFeedback = prevFeedback.concat(feedback);
        setPrevGuesses(nextGuesses);
        setPrevFeedback(nextFeedback);

        if (feedback === "GGGGG") {
          setShowVictory(true);
        } else if (wordsSubmitted + 1 >= GRID_ROWS) {
          setShowDefeat(true);
        }

        setWordsSubmitted(function (prev) {
          return prev + 1;
        });
        setCurrentWord("");
      } else {
        setShowInvalidWord(true);
      }

      Module._free(currentWordCPP);
    }

    function handleKey(key) {
      if (!runtimeReady || showVictory || showDefeat) {
        return;
      }

      if (key === "Backspace") {
        setCurrentWord(function (prev) {
          return prev.slice(0, -1);
        });
        return;
      }

      if (key === "Enter") {
        submitGuess();
        return;
      }

      if (key.length === 1 && currentWord.length < GRID_COLS) {
        setCurrentWord(function (prev) {
          if (prev.length >= GRID_COLS) {
            return prev;
          }
          return prev + key.toUpperCase();
        });
      }
    }

    useEffect(() => {
      updateBoardWithCurrentWord(currentWord, wordsSubmitted);
    }, [currentWord, wordsSubmitted]);

    return React.createElement(
      React.Fragment,
      null,
      React.createElement(
        "div",
        { id: "container" },
        React.createElement(
          "div",
          { id: "game" },
          React.createElement(
            "header",
            null,
            React.createElement("h1", { className: "title" }, "Wordle-PP"),
            React.createElement(
              "div",
              { className: "header-controls" },
                React.createElement(
                  "div",
                  { className: "hard-mode-toggle" },
                React.createElement(
                  "label",
                  { className: "switch" },
                  React.createElement("input", {
                    type: "checkbox",
                    checked: hardMode,
                    onChange: function () {
                      setHardMode(function (prev) {
                        return !prev;
                      });
                    }
                  }),
                  React.createElement("span", { className: "slider round" })
                ),
                React.createElement("span", null, "Impossible Mode")
              ),
              React.createElement(
                "button",
                {
                  id: "restart-game",
                  "aria-label": "Restart Game",
                  onClick: resetGame
                },
                React.createElement("img", {
                  src: "icons/restartIconBlackBg.png",
                  alt: "Restart Icon"
                })
              )
            )
          ),
          React.createElement(
            "div",
            { id: "board-container" },
            React.createElement(
              "div",
              { id: "wordGrid" },
              board.map(function (row, rowIndex) {
                return React.createElement(
                  "div",
                  { className: "grid-row", key: "row-" + rowIndex },
                  row.map(function (cell, colIndex) {
                    const classes = ["grid-cell"];
                    if (cell.flip) {
                      classes.push("flip-animation");
                    }
                    return React.createElement(
                      "div",
                      {
                        className: classes.join(" "),
                        id: cell.id,
                        key: "cell-" + rowIndex + "-" + colIndex,
                        style: cell.colour
                          ? { backgroundColor: cell.colour }
                          : undefined
                      },
                      cell.letter
                    );
                  })
                );
              })
            )
          ),
          React.createElement(
            "div",
            { id: "keyboard-container" },
            KEYBOARD_ROWS.map(function (row, rowIndex) {
              return React.createElement(
                "div",
                { className: "keyboard-row", key: "keyboard-row-" + rowIndex },
                row.map(function (keyName) {
                  const isWide = keyName === "Enter" || keyName === "Backspace";
                  const keyClass = keyboardState[keyName] || "";
                  const label = keyName === "Backspace" ? "DEL" : keyName.toUpperCase();
                  const buttonClasses = ["keyboard-key"];
                  if (isWide) {
                    buttonClasses.push("wide-button");
                  }
                  if (keyClass) {
                    buttonClasses.push(keyClass);
                  }
                  return React.createElement(
                    "button",
                    {
                      className: buttonClasses.join(" "),
                      id: "key-" + keyName,
                      key: "key-" + keyName,
                      onClick: function () {
                        handleKey(keyName);
                      }
                    },
                    label
                  );
                })
              );
            })
          )
        )
      ),
      showVictory &&
        React.createElement(
          "div",
          { id: "victory-popup", className: "popup-end-container" },
          React.createElement(
            "div",
            { className: "popup-content" },
            React.createElement("h2", null, "You Win!"),
            React.createElement(
              "p",
              null,
              "Congratulations, you guessed the word!"
            ),
            React.createElement(
              "button",
              {
                onClick: function () {
                  setShowVictory(false);
                }
              },
              "Close"
            ),
            React.createElement(
              "button",
              { onClick: resetGame },
              "Play Again"
            )
          )
        ),
      showDefeat &&
        React.createElement(
          "div",
          { id: "defeat-popup", className: "popup-end-container" },
          React.createElement(
            "div",
            { className: "popup-content" },
            React.createElement("h2", null, "Game Over"),
            React.createElement(
              "p",
              null,
              "Sorry, you didn't guess the word."
            ),
            React.createElement(
              "button",
              {
                onClick: function () {
                  setShowDefeat(false);
                }
              },
              "Close"
            ),
            React.createElement(
              "button",
              { onClick: resetGame },
              "Try Again"
            )
          )
        ),
      React.createElement(
        "div",
        {
          id: "popup-container",
          style: { display: showInvalidWord ? "flex" : "none" }
        },
        React.createElement("div", { id: "popup-message" }, "Invalid Word!")
      )
    );
  }

  ReactDOM.createRoot(document.getElementById("root")).render(
    React.createElement(WordleApp)
  );
})();
