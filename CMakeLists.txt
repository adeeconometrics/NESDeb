cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pedantic")

project(NESDeb CXX)

file(GLOB_RECURSE SRC src/*.cpp)
include_directories(include)

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(
	${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
)
