cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(MdVis)

add_executable(mdvis 
	src/Defines.h 
	src/GL.h 
	src/GL.cpp 
	src/main.cpp 
	src/Spline.hpp
	src/xoshiro.h
	src/OBJ_Loader.h
)

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glm-0.9.9.8")
include_directories("${CMAKE_SOURCE_DIR}/libs/glm-0.9.9.8/include")
target_link_libraries(mdvis glm)

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glew-2.1.0/build/cmake")
add_compile_definitions(GLEW_STATIC)
include_directories("${CMAKE_SOURCE_DIR}/libs/glew-2.1.0/include")
target_link_libraries(mdvis  glew_s)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

add_subdirectory("${CMAKE_SOURCE_DIR}/libs/glfw-3.3.2")
include_directories("${CMAKE_SOURCE_DIR}/libs/glfw-3.3.2/include")
target_link_libraries(mdvis glfw ${GLFW_LIBRARIES})