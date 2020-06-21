cmake_minimum_required(VERSION 3.11)

project(MdVis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

include(FetchContent)

add_executable(mdvis 
	src/Defines.h 
	src/GL.h 
	src/GL.cpp 
	src/main.cpp 
	src/Spline.hpp
	src/xoshiro.h
	src/OBJ_Loader.h
)

set(GLAD_API "gl=4.3" CACHE STRING "API type/version pairs, like \"gl=4.6,gles=\", no version means latest")
FetchContent_Declare(
	glad
	GIT_REPOSITORY https://github.com/Dav1dde/glad.git
	GIT_TAG        v0.1.33
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
	FetchContent_Populate(glad)
	add_subdirectory("${glad_SOURCE_DIR}" "${glad_BINARY_DIR}")
endif()

message(STATUS "Building natively")

if (MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -fno-omit-frame-pointer -D_GLIBCXX_DEBUG -fsanitize=address -fsanitize=leak -fsanitize=pointer-compare -fsanitize=pointer-subtract -fsanitize=undefined" )
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG -flto")
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glfw-3.3.2")
include_directories("${CMAKE_SOURCE_DIR}/libs/glfw-3.3.2/include")

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glm-0.9.9.8")
include_directories("${CMAKE_SOURCE_DIR}/libs/glm-0.9.9.8/include")

target_link_libraries(mdvis
		glfw
		${GLFW_LIBRARIES}
		glad
		glm
		${CMAKE_DL_LIBS}
)
