cmake_minimum_required(VERSION 3.10)
project(huffman)
include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_library(huffman.lib STATIC reader.h reader.cpp writer.h writer.cpp
        huffman.h huffman.cpp)

add_executable(huffman tool.cpp)
add_executable(huffman_testing tests.cpp gtest/gtest.h gtest/gtest-all.cc gtest/gtest_main.cc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

target_link_libraries(huffman huffman.lib)
target_link_libraries(huffman_testing huffman.lib)

target_link_libraries(huffman_testing -lpthread)