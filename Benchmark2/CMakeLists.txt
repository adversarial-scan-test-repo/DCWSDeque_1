cmake_minimum_required(VERSION 3.10)
project(Benchmark2)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(src)
add_subdirectory(test)

include_directories(lib/googletest-master/googletest/include)
include_directories(lib/googletest-master/googlemock/include)
