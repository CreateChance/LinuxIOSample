cmake_minimum_required(VERSION 3.15)
project(LinuxIO C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(bio_client ioModel/bio_client.c)
add_executable(bio_server ioModel/bio_server.c)
add_executable(nio_server ioModel/nio_server.c)
add_executable(select_server ioModel/select_server.c)
add_executable(read_server netBase/05_read_server.c)
add_executable(read_client netBase/05_read_client.c)
add_executable(06_udp_server netBase/06_udp_server.c)
add_executable(06_udp_client netBase/06_udp_client.c)
add_executable(07_unix_stream_server netBase/07_unix_stream_server.c)
add_executable(07_unix_stream_client netBase/07_unix_stream_client.c)
add_executable(07_unix_datagram_server netBase/07_unix_datagram_server.c)
add_executable(07_unix_datagram_client netBase/07_unix_datagram_client.c)
add_executable(10_close_server netBase/10_close_server.c)
add_executable(10_close_client netBase/10_close_client.c)
