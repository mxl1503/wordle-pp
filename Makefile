EMSDK_IMAGE := emscripten/emsdk:3.1.71
UID_GID := $(shell id -u):$(shell id -g)
TEST_IMAGE := wordle-pp-test

SRC := cpp-logic/src/wasm_exports.cpp cpp-logic/src/engine.cpp cpp-solver/src/solver.cpp
OUT := frontend/wordle.js

EXPORTED_FUNCTIONS := ["_createNewGame","_makeGuess","_validateGuess","_makeHardModeGuess","_recommendNextGuess","_freeCString","_free","_malloc"]
EXPORTED_RUNTIME_METHODS := ["ccall","cwrap","UTF8ToString","stringToUTF8","setValue"]

.PHONY: build build-clean serve clean test-cpp test-cpp-docker clangd-setup format-cpp check-format-cpp install-hooks

build:
	docker run --rm \
		-u $(UID_GID) \
		-v $(PWD):/src \
		-w /src \
		$(EMSDK_IMAGE) \
		em++ -std=c++17 -O3 \
		-Icpp-logic/include \
		-Icpp-solver/include \
		--preload-file cpp-logic/data/wordlists/answerList.txt \
		--preload-file cpp-logic/data/wordlists/guessList.txt \
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

test-cpp-docker:
	docker build -f Dockerfile.test -t $(TEST_IMAGE) .
	docker run --rm \
		-u $(UID_GID) \
		-v $(PWD):/src \
		-w /src \
		$(TEST_IMAGE) \
		sh -lc "cmake -S . -B build/tests-docker -DBUILD_TESTING=ON && cmake --build build/tests-docker && ctest --test-dir build/tests-docker --output-on-failure"

clangd-setup:
	cmake -S . -B build/clangd -DBUILD_TESTING=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cp build/clangd/compile_commands.json ./compile_commands.json

format-cpp:
	@files="$$(git ls-files '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hh' '*.hpp' '*.hxx')"; \
	if [ -n "$$files" ]; then clang-format -i $$files; fi

check-format-cpp:
	@files="$$(git ls-files '*.c' '*.cc' '*.cpp' '*.cxx' '*.h' '*.hh' '*.hpp' '*.hxx')"; \
	if [ -n "$$files" ]; then clang-format --dry-run --Werror $$files; fi

install-hooks:
	mkdir -p .git/hooks
	cp .githooks/pre-commit .git/hooks/pre-commit
	chmod +x .git/hooks/pre-commit
	@echo "Installed pre-commit hook: .git/hooks/pre-commit"

clean:
	rm -f frontend/wordle.js frontend/wordle.wasm frontend/wordle.data
