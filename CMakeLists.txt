cmake_minimum_required(VERSION 3.29)
project(stest C)

set(CMAKE_C_STANDARD 99)

add_executable(stest main.c lib.c stest.c)

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${CMAKE_SOURCE_DIR}/stest.ld")
