cmake_minimum_required(VERSION 2.8)
project('final-project')

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
add_executable (main consensus.c)
target_link_libraries(main pthread)