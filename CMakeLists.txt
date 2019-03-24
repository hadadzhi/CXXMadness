cmake_minimum_required(VERSION 3.13)
project(cxx_madness)

# Set flags for standard C++17
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pedantic-errors")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif ()

include_directories(src)

add_executable(cxx_madness
        src/main.cpp
        src/StupidString.cpp
        src/StupidString.h)
