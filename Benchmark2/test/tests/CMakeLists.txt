include_directories(../lib/googletest/include/gtest)

add_executable(runTests TestBenchmark2Sequential.cpp TestBenchmark2Threads.cpp)

target_link_libraries(runTests gtest gtest_main)