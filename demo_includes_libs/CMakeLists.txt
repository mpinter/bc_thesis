cmake_minimum_required(VERSION 2.8)
# Project Name
PROJECT(BAK_TEST)

add_subdirectory("${CMAKE_SOURCE_DIR}/lvis")

#########################################################
# GENERAL INCLUDE
#########################################################
include_directories("${CMAKE_SOURCE_DIR}/lvis/include")

#########################################################
# FIND EXTERNAL INCLUDE FILES
#########################################################
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)
find_package(DevIL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
if(NOT GLEW_FOUND)
    message(ERROR "GLEW not found!")
endif(NOT GLEW_FOUND)
if(NOT SFML_FOUND)
    message(ERROR "SFML not found!")
endif(NOT SFML_FOUND)
if(NOT IL_FOUND)
    message(ERROR "DevIL not found!")
endif(NOT IL_FOUND)
if(NOT ASSIMP_FOUND)
    message(ERROR "ASSIMP not found!")
endif(NOT ASSIMP_FOUND)
include_directories(${OpenGL_INCLUDE_DIRS})
include_directories(${SFML_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${IL_INCLUDE_DIR})
include_directories (${Boost_INCLUDE_DIRS})
include_directories(${ASSIMP_INCLUDE_DIR})
add_definitions(${OpenGL_DEFINITIONS})

#########################################################
# G++ FLAGS
#########################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -std=c++0x")

#########################################################
# Include Files
#########################################################
add_executable(test main-test.cpp)

########################################################
# Linking & Executable creation
#########################################################
link_directories("${CMAKE_SOURCE_DIR}/lvis")

target_link_libraries(test lvis)