cmake_minimum_required(VERSION 3.21)
project(box2d_ball LANGAUGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STARDARD_REQUIRED ON)

find_package(box2d CONFIG REQUIRED)

add_executable(ball src/main.cpp)
target_link_libraries(ball PRIVATE box2d::box2d) 
