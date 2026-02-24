EMSDK_IMAGE := emscripten/emsdk:3.1.71
UID_GID := $(shell id -u):$(shell id -g)

SRC := wordle.cpp
OUT := wordle.js

EXPORTED_FUNCTIONS := ["_getAnswerList","_getGuessList","_createNewGame","_makeGuess","_evaluateGuess","_validateGuess","_isMatch","_filterAnswerList","_findWorstFeedback","_makeHardModeGuess","_free","_malloc"]
EXPORTED_RUNTIME_METHODS := ["ccall","cwrap","UTF8ToString","stringToUTF8","setValue"]

.PHONY: build build-clean serve clean

build:
	docker run --rm \
		-u $(UID_GID) \
		-v $(PWD):/src \
		-w /src \
		$(EMSDK_IMAGE) \
		em++ -std=c++17 -O3 \
		--preload-file wordLists/answerlist.txt \
		--preload-file wordLists/guesslist.txt \
		$(SRC) -o $(OUT) -s WASM=1 \
		-s EXPORTED_FUNCTIONS='$(EXPORTED_FUNCTIONS)' \
		-s EXPORTED_RUNTIME_METHODS='$(EXPORTED_RUNTIME_METHODS)'

build-clean: clean build

serve:
	docker compose up --build web

clean:
	rm -f wordle.js wordle.wasm wordle.data
