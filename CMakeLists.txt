cmake_minimum_required(VERSION 3.21)

project(AstroX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

# Setting parameters for raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # or games


add_library(
	custom_button
	src/custom_button.cpp
	src/custom_button.hpp
)
	
add_subdirectory(libs/raylib)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(custom_button PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE custom_button)