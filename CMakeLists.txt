cmake_minimum_required(VERSION 3.15)
project(LinuxIO C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

add_executable(bio_client ioModel/bio_client.c)
add_executable(bio_server ioModel/bio_server.c)
add_executable(nio_server ioModel/nio_server.c)
add_executable(select_server ioModel/select_server.c)
add_executable(read_server netBase/read_server.c)
add_executable(read_client netBase/read_client.c)
