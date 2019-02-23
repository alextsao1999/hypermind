# Created By 曹顺 
cmake_minimum_required(VERSION 3.13)
project(HyperMind)

set(CMAKE_CXX_STANDARD 14)

set(DIRS . obj ast)

include_directories(.)

foreach (DIR ${DIRS})
    aux_source_directory(${DIR} FILES)
endforeach ()

add_executable(HyperMind ${FILES})
