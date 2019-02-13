cmake_minimum_required(VERSION 3.13)
project(HyperMind)

set(CMAKE_CXX_STANDARD 14)
include_directories(.)

add_executable(HyperMind main.cpp lexer.cpp parser.cpp parser.h utils.cpp utils.h buffer.cpp buffer.h vm.cpp vm.h obj/string.cpp obj/string.h)