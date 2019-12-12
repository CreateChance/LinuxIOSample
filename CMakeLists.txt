cmake_minimum_required(VERSION 3.15)
project(LinuxIO C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(bio_server bio_server.c)
add_executable(bio_client bio_client.c)