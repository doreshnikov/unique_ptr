cmake_minimum_required(VERSION 3.10)
project(unique_ptr)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,leak,undefined")

add_executable(unique_ptr testing.cpp unique_ptr.hpp)