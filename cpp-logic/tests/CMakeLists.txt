find_package(GTest CONFIG QUIET)
if(NOT GTest_FOUND)
  find_package(GTest REQUIRED)
endif()

add_executable(wordle_engine_test
  engine_test.cpp
)

target_link_libraries(wordle_engine_test
  PRIVATE
    wordle_engine
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(wordle_engine_test)
