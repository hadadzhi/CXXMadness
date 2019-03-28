cmake_minimum_required(VERSION 3.13)
project(cxx_madness)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic-errors")

include_directories(src)

add_executable(cxx_madness
        src/main.cpp
        src/StupidString.cpp
        src/StupidString.h)
