cmake_minimum_required(VERSION 4.0)
project(cpp_playground)

set(CMAKE_CXX_STANDARD 20)

add_executable(cpp_playground main.cpp
        Allocator.cpp
        Allocator.h)
