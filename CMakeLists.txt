cmake_minimum_required(VERSION 3.15)
project(test_threads)

set(CMAKE_CXX_STANDARD 14)

add_executable(test_threads main.cpp)

target_link_libraries(test_threads pthread)