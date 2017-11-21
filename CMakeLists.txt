cmake_minimum_required(VERSION 2.8)
project(procedural_planets)

# Directories
link_directories("/usr/local/lib" "/opt/local/lib")
include_directories("/usr/local/include" "/opt/local/include")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# Flags
set(CMAKE_CXX_FLAGS "--std=c++11 -g -fmax-errors=1")

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

# GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

# GLFW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW3 REQUIRED glfw3)
include_directories(${GLFW3_INCLUDE_DIRS})

# glm
find_package(glm QUIET)
include_directories(${glm_INCLUDE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLM_FORCE_SIZE_FUNC=1 -DGLM_FORCE_RADIANS=1")

list(APPEND GL ${OPENGL_gl_LIBRARY} ${GLFW3_STATIC_LIBRARIES} ${GLEW_LIBRARIES})

message(STATUS "USING C++ Compiler ${CMAKE_CXX_COMPILER}")
message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")
message(STATUS "GLEW_LIBRARIES=${GLEW_LIBRARIES}")
message(STATUS "GLFW_LIBRARIES=${GLFW3_STATIC_LIBRARIES}")

string(CONCAT pwd ${CMAKE_SOURCE_DIR} "/src")
set(src "")
aux_source_directory(${pwd} src)
add_executable(procgen ${src})
message(STATUS "progen added ${src}")

target_link_libraries(procgen ${GL})
