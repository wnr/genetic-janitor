cmake_minimum_required(VERSION 3.2)
project(genetic_janitor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(genetic_janitor ${SOURCE_FILES} game.h game.cpp randomUtil.h randomUtil.cpp genetic.h genetic.cpp)