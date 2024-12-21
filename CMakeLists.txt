cmake_minimum_required(VERSION 3.28)
project(Deque)

set(CMAKE_CXX_STANDARD 23)

add_executable(Deque main.cpp deque.h small_tests.h)
add_executable(Tests tests.cpp)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

