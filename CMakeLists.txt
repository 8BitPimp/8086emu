cmake_minimum_required(VERSION 3.4)
project(8086emu)

if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_subdirectory(distorm)
add_subdirectory(source)
