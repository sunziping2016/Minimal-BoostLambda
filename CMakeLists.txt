cmake_minimum_required(VERSION 3.5)
project(lambda)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")

set(SOURCE_FILES main.cpp lambda.h)
add_executable(lambda ${SOURCE_FILES})