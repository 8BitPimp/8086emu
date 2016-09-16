cmake_minimum_required(VERSION 3.4)

if (NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

file(GLOB C_FILES *.cpp)
file(GLOB H_FILES *.h)

add_executable(8086emu ${C_FILES} ${H_FILES} bootsector.cpp)
target_link_libraries(8086emu PUBLIC distorm)
