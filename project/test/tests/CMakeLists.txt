include_directories(../lib/googletest/include/gtest)

add_executable(runTests CircularArrayTest.cpp DCWSDequeTest.cpp CircularArrayShrinkTest.cpp DCWSDequeThreadsTest.cpp)

target_link_libraries(runTests gtest gtest_main)
#target_link_libraries(runTests main)
