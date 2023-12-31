Module.onRuntimeInitialized = async _ => {
  let targetWordPtr = Module._createNewGame();
  let targetWord = Module.UTF8ToString(targetWordPtr);
  let currentWord = '';
  let wordsSubmitted = 0;

  // Relevant to hard mode only
  let hardMode = false;
  let prevGuesses = [];
  let prevFeedback = [];
  
  // Function to toggle impossible/hard mode on and off
  function toggleHardMode() {
    hardMode = !hardMode;
    resetGame();
  }

  // Function to allocate an array of strings in Javascript as C-style arrays
  function allocateStringArray(strings) {
    const ptrs = strings.map(str => {
      const buffer = Module._malloc(str.length + 1);
      Module.stringToUTF8(str, buffer, str.length + 1);
      return buffer;
    });

    // Allocate memory for the array of pointers
    const arrayPtr = Module._malloc(ptrs.length * 4);
    ptrs.forEach((ptr, index) => {
      // Set the pointer in the array
      Module.setValue(arrayPtr + index * 4, ptr, 'i32');
    });

    return [arrayPtr, ptrs];
  }

  // Function to help setup C-style arrays before passing it into the C++ function that makes guesses
  function hardModeGuessHelper(prevGuesses, prevFeedback, currentWordCPP) {
    const [prevGuessesArrPtr, ptrsOne] = allocateStringArray(prevGuesses);
    const [prevFeedbackArrPtr, ptrsTwo] = allocateStringArray(prevFeedback);

    const returnVal = Module._makeHardModeGuess(prevGuessesArrPtr, prevFeedbackArrPtr, currentWordCPP, prevGuesses.length);

    ptrsOne.forEach(ptr => Module._free(ptr));
    ptrsTwo.forEach(ptr => Module._free(ptr));
    Module._free(prevGuessesArrPtr);
    Module._free(prevFeedbackArrPtr);

    return returnVal;
  }

  // Function to handle keyboard inputs
  function handleKey(key) {
    let gameWon = false;
    if (key === 'Backspace') {
      // Handle Backspace key to remove the last character from the current word
      currentWord = currentWord.slice(0, -1);
    } else if (key === 'Enter') {
      processEnterKey(gameWon);
    } else if (key.length === 1 && currentWord.length < 5) {
      // Handle letter keys (A-Z) to add letters to the current word
      currentWord += key.toUpperCase();
    }

    // Update the display with the current word
    updateGameState(gameWon);
  }

  // Function to help process what happens when the enter key is pressed
  function processEnterKey(gameWon) {
    // Convert to a C-Style string before calling functions in C++
    const currentWordCPP = Module._malloc(currentWord.length + 1);
    Module.stringToUTF8(currentWord.toLowerCase(), currentWordCPP, currentWord.length + 1);

    // Check if guess is a valid word
    if (currentWord.length === 5 && Module._validateGuess(currentWordCPP)) {
      // Again convert to a C-Style string before calling helper functions, then update grid
      // accordingly
      const feedbackPtr = hardMode ? hardModeGuessHelper(prevGuesses, prevFeedback, currentWordCPP) : Module._makeGuess(currentWordCPP, targetWordPtr);
      const feedback = Module.UTF8ToString(feedbackPtr);
      gameLogic(feedback, gameWon);

      // Free strings
      Module._free(feedbackPtr);
    } else {
      showInvalidWordPopup();
    }

    Module._free(currentWordCPP);
  }

  // Function that implements game logic upon receiving feedback from a guess
  function gameLogic(feedback, gameWon) {
    // Update previous guesses and feedback arrays
    prevGuesses.push(currentWord.toLowerCase());
    prevFeedback.push(feedback);

    // Update grid and keyboard colours
    updateGridColours(feedback);
    updateKeyboard(feedback, currentWord);

    // Check if game is over
    if (feedback === 'GGGGG') {
      gameWon = true;
      showVictoryPopUp();
    }

    if (!gameWon) {
      currentWord = '';
      wordsSubmitted++;
    }
  }

  // Function that checks the game state
  function updateGameState(gameWon) {
    if (gameWon) {
      showVictoryPopUp();
    } else if (wordsSubmitted >= 6) {
      showDefeatPopUp();
    } else {
      updateDisplay(currentWord);
    }
  }

  // Function that returns a string based on input character
  function getColourFromFeedback(char) {
    switch (char) {
      case 'G': return 'rgb(106, 169, 100)';
      case 'Y': return 'yellow';
      default: return 'rgb(120, 124, 126)';
    }
  }

  // Function to update grid colours according to feedback string
  function updateGridColours(feedbackString) {
    let index = 0;
    for (const character of feedbackString) {
      const cell = document.getElementById(`cell-${wordsSubmitted}-${index}`);
      const colour = getColourFromFeedback(character);
  
      // Trigger flip animation
      cell.classList.add('flip-animation');
  
      // Change the background color of the cell in the middle of the flip animation
      setTimeout(() => {
        cell.style.backgroundColor = colour;
      }, 400);
  
      // Remove the animation class
      cell.addEventListener('animationend', () => {
        cell.classList.remove('flip-animation');
      });
  
      index++;
    }
  }

  // Function to update the display with the current word
  function updateDisplay(word) {
    let index = 0;
    for (const letter of word) {
      const displayElement = document.getElementById(`cell-${wordsSubmitted}-${index}`);
      displayElement.innerText = letter;
      index++;
    }

    while (index < 5) {
      const displayElement = document.getElementById(`cell-${wordsSubmitted}-${index}`);
      displayElement.innerText = null;
      index++;
    }
  }

  // Function to update keyboard colours based on feedback string
  function updateKeyboard(feedback, guess) {
    for (let i = 0; i < guess.length; i++) {
      const letter = guess[i];
      const feedbackChar = feedback[i];
      const keyElement = document.getElementById(`key-${letter.toUpperCase()}`);

      if (feedbackChar === 'G') {
        keyElement.classList.add('correct');
      } else if (feedbackChar === 'Y') {
        keyElement.classList.add('present');
      } else if (feedbackChar === 'B') {
        keyElement.classList.add('absent');
      }
    }
  }

  // Function to show the victory popup
  function showVictoryPopUp() {
    document.getElementById('victory-popup').style.display = 'flex';
  }

  // Function to show the defeat popup
  function showDefeatPopUp() {
    document.getElementById('defeat-popup').style.display = 'flex';
  }

  // Function to create the invalid word pop-up
  function showInvalidWordPopup() {
    const popup = document.getElementById('popup-container');
    popup.style.display = 'flex';
    setTimeout(() => {
      hideInvalidWordPopup();
    }, 1000);
  }
  
  // Function to hide the invalid word pop-up
  function hideInvalidWordPopup() {
    const popup = document.getElementById('popup-container');
    popup.style.display = 'none';
  }

  // Function to toggle the hard mode info pop-up
  function toggleInfoPopup() {
    var popup = document.getElementById("info-popup");
    popup.style.display = popup.style.display === 'none' ? 'flex' : 'none';
  }

  // Function to close victory/defeat popup
  function closePopUp(popupId) {
    document.getElementById(popupId).style.display = 'none';
  }

  // Function to reset the game state
  function resetGame() {
    // Close any open popups
    closePopUp('victory-popup');
    closePopUp('defeat-popup');

    // Reset game state variables
    wordsSubmitted = 0;
    currentWord = '';
    prevGuesses = [];
    prevFeedback = [];

    targetWordPtr = Module._createNewGame();
    targetWord = Module.UTF8ToString(targetWordPtr);

    // Clear the game board
    for (let row = 0; row < 6; row++) {
      for (let col = 0; col < 5; col++) {
        const cell = document.getElementById(`cell-${row}-${col}`);
        cell.innerText = '';
        cell.style.backgroundColor = 'darkgray';
      }
    }

    // Reset the keyboard (if needed)
    // Assuming each key has an ID like 'key-Q', 'key-W', etc.
    const keys = document.querySelectorAll('.keyboard-key');
    keys.forEach(key => {
      key.classList.remove('correct', 'present', 'absent');
    });
  }
  
  // Event listener for keyboard inputs
  document.addEventListener('keydown', (event) => {
    if (event.key === 'Backspace' || event.key === 'Enter' || /^[A-Z]$/i.test(event.key)) {
      handleKey(event.key);
    }
  });

  // Allow functions to be accessed globally
  window.handleKey = handleKey;
  window.closePopUp = closePopUp;
  window.resetGame = resetGame;
  window.toggleHardMode = toggleHardMode;
  window.toggleInfoPopup = toggleInfoPopup;
};
