cmake_minimum_required(VERSION 3.15)
project(redlang)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_library(redlang_parser lang/parser/parser.cpp lang/parser/parser.hpp
        lang/parser/types.cpp lang/parser/types.hpp)

add_executable(redlang main.cpp)

target_link_libraries(redlang redlang_parser)