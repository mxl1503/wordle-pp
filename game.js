Module.onRuntimeInitialized = async _ => {
  const targetWordPtr = Module._createNewGame();
  const targetWord = Module.UTF8ToString(targetWordPtr);
  console.log(targetWord);

  let currentWord = '';
  let wordsSubmitted = 0;

  // Function to handle keyboard inputs
  function handleKey(key) {
    if (key === 'Backspace') {
      // Handle Backspace key to remove the last character from the current word
      currentWord = currentWord.slice(0, -1);
    } else if (key === 'Enter') {
      // Convert to a C-Style string before calling functions in C++
      const currentWordCPP = Module._malloc(currentWord.length + 1);
      Module.stringToUTF8(currentWord.toLowerCase(), currentWordCPP, currentWord.length + 1);
      
      // Check if guess is a valid word
      if (currentWord.length === 5 && Module._validateGuess(currentWordCPP)) {
        // Again convert to a C-Style string before calling helper functions, then update grid
        // accordingly
        const targetWordCPP = Module._malloc(targetWord.length + 1);
        Module.stringToUTF8(targetWord.toLowerCase(), targetWordCPP, targetWord.length + 1);
        const feedbackPtr = Module._makeGuess(currentWordCPP, targetWordCPP);
        const feedback = Module.UTF8ToString(feedbackPtr);
        updateGridColours(feedback);

        // Free strings
        Module._free(feedbackPtr);
        Module._free(targetWordCPP);
        
        currentWord = '';
        wordsSubmitted++;
      } else {
        // Invalid word, show the invalid word popup
        showInvalidWordPopup();
        console.log('running');
      }

      // Free string
      Module._free(currentWordCPP)
    } else if (key.length === 1 && currentWord.length < 5) {
      // Handle letter keys (A-Z) to add letters to the current word
      currentWord += key.toUpperCase();
    }

    // Update the display with the current word
    updateDisplay(currentWord);
  }

  // Allow the handleKey function to be accesssed globally
  window.handleKey = handleKey;
  
  function updateGridColours(feedbackString) {
    let index = 0;
    for (const character of feedbackString) {
      const cell = document.getElementById(`cell-${wordsSubmitted}-${index}`);
      const colour = getColorFromFeedback(character);
  
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

  // Function that returns a string based on input character
  function getColorFromFeedback(char) {
    switch (char) {
      case 'G': return 'rgb(106, 169, 100)';
      case 'Y': return 'yellow';
      default: return 'rgb(120, 124, 126)';
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
  
  // Event listener for keyboard inputs
  document.addEventListener('keydown', (event) => {
    const key = event.key;
    console.log(key);
    handleKey(key);
  });
};

