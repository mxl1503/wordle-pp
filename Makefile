EMSDK_IMAGE := emscripten/emsdk:3.1.71
UID_GID := $(shell id -u):$(shell id -g)

SRC := cpp-logic/src/wasm_exports.cpp cpp-logic/src/engine.cpp
OUT := frontend/wordle.js

EXPORTED_FUNCTIONS := ["_createNewGame","_makeGuess","_validateGuess","_makeHardModeGuess","_freeCString","_free","_malloc"]
EXPORTED_RUNTIME_METHODS := ["ccall","cwrap","UTF8ToString","stringToUTF8","setValue"]

.PHONY: build build-clean serve clean test-cpp install-hooks

build:
	docker run --rm \
		-u $(UID_GID) \
		-v $(PWD):/src \
		-w /src \
		$(EMSDK_IMAGE) \
		em++ -std=c++17 -O3 \
		-Icpp-logic/include \
		--preload-file wordLists/answerlist.txt \
		--preload-file wordLists/guesslist.txt \
		$(SRC) -o $(OUT) -s WASM=1 \
		-s EXPORTED_FUNCTIONS='$(EXPORTED_FUNCTIONS)' \
		-s EXPORTED_RUNTIME_METHODS='$(EXPORTED_RUNTIME_METHODS)'

build-clean: clean build

serve:
	docker compose up --build web

test-cpp:
	cmake -S . -B build/tests -DBUILD_TESTING=ON
	cmake --build build/tests
	ctest --test-dir build/tests --output-on-failure

install-hooks:
	mkdir -p .git/hooks
	cp .githooks/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
	@echo "Installed pre-commit hook: .git/hooks/pre-commit"

clean:
	rm -f frontend/wordle.js frontend/wordle.wasm frontend/wordle.data
