cmake_minimum_required(VERSION 2.8)
project('Final_Project')

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall")
add_executable (main consensus.c cwork.c server.c)
