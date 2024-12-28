cmake_minimum_required(VERSION 3.10)
project(jewel)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
    src/lexer.cpp
    src/main.cpp
)

add_executable(jewel ${SOURCES})