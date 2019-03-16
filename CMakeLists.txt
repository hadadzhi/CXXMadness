cmake_minimum_required(VERSION 3.13)
project(cxx_madness)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(cxx_madness
        src/main.cpp
        src/StupidString.cpp
        src/StupidString.h)
