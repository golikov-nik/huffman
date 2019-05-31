cmake_minimum_required(VERSION 3.14)
project(huffman)
include_directories(${PROJECT_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_library(huffman.lib STATIC reader.h reader.cpp writer.h writer.cpp
        huffman.h huffman.cpp)

add_executable(huffman tool.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")

target_link_libraries(huffman huffman.lib)
