cmake_minimum_required(VERSION 3.3)
project(SPL_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(C:/installation/path/include/boost-1_60)


set(SOURCE_FILES ConnectionHandler.h ConnectionHandler.cpp echoClient.cpp)
add_executable(SPL_client ${SOURCE_FILES} ConnectionHandler.h)