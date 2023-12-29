cmake_minimum_required(VERSION 3.19)
project(Chess_problem)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

add_executable(Chess_problem main.cpp)