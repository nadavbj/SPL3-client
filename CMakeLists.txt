cmake_minimum_required(VERSION 3.3)
project(SPL_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(SPL_client ${SOURCE_FILES})