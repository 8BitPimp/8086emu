cmake_minimum_required(VERSION 3.4)

if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

file(GLOB C_FILES src/*.c)
file(GLOB H_FILES src/*.h)
file(GLOB I_FILES include/*.h)

add_library(distorm ${C_FILES} ${H_FILES})
