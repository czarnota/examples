cmake_minimum_required(VERSION 3.1)

project(helloworld2)

set(CMAKE_CXX_STANDARD 17)

file(GLOB helloworld2_sources *.cpp)
add_executable(helloworld2 ${helloworld2_sources})
