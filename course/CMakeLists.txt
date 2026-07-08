#
# To edit CMakeLists.txt after modifying, run `cmake -S . -B build --fresh`.
# Then, recompile the project with `cmake --build build`.
#
cmake_minimum_required(VERSION 3.15)
project(course C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
	sfml
	GIT_REPOSITORY 	https://github.com/SFML/SFML.git
	GIT_TAG 	3.0.2
)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
	box2d
	GIT_REPOSITORY 	https://github.com/erincatto/box2d.git
	GIT_TAG 	v3.1.1
)
set(BOX2D_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(box2d sfml)

add_executable(course course.cpp)

get_target_property(EXEC_DIR course RUNTIME_OUTPUT_DIRECTORY)

message(STATUS "Target runtime output dir: ${EXEC_DIR}")
message(STATUS "Standard binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(course PRIVATE
	sfml-graphics
	sfml-window
	sfml-system
	box2d
)

add_custom_command(
	TARGET course
	POST_BUILD
	COMMAND course
	COMMENT "Executing after building..."
	VERBATIM
)
