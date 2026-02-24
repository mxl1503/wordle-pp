cmake_minimum_required(VERSION 3.20)
project(wordle_pp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(wordle_engine STATIC
  cpp-logic/src/engine.cpp
)

target_include_directories(wordle_engine
  PUBLIC
    ${PROJECT_SOURCE_DIR}/cpp-logic/include
)

add_executable(hard_mode_testing
  hardModeTesting.cpp
)
target_link_libraries(hard_mode_testing PRIVATE wordle_engine)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(cpp-logic/tests)
endif()
