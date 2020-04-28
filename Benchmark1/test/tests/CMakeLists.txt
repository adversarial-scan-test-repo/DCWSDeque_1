include_directories(../lib/googletest/include/gtest ../../src/)

add_executable(runTest Benchmark1Sequential.cpp Benchmark1Threads.cpp)

target_link_libraries(runTest gtest gtest_main)