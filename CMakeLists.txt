cmake_minimum_required(VERSION 3.16)

project(NESDeb CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pedantic")

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(PNG REQUIRED)
find_package(Threads REQUIRED)


if(UNIX)
        # We are not using the Apple's framework version, but X11's
        include_directories( /usr/X11/include )
        link_directories( /usr/X11/lib )
        set (OPENGL_LIBRARIES GL GLU X11)
endif(UNIX)

include_directories(
	${OPENGL_INCLUDE_DIRS} 
	${GLUT_INCLUDE_DIRS}
	${X11_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS} 
)


file(GLOB_RECURSE SRC src/*.cpp)
include_directories(include)

add_executable(${PROJECT_NAME} ${SRC})


target_link_libraries(
	${PROJECT_NAME}
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES} 
	${X11_LIBRARIES}
	${GLUT_LIBRARY}
	${PNG_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
)

target_include_directories(
	${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include 
)
