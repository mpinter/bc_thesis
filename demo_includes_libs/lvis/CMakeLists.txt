cmake_minimum_required(VERSION 2.8)

#########################################################
# GENERAL INCLUDE & LINK
#########################################################
include_directories("${CMAKE_SOURCE_DIR}/lvis/include")
include_directories("${CMAKE_SOURCE_DIR}/includeAll")
include_directories("${CMAKE_SOURCE_DIR}/includeAll/IL")
link_directories("${CMAKE_SOURCE_DIR}/libAll")

#########################################################
# G++ FLAGS
#########################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -std=c++0x")

#########################################################
# Include Files
#########################################################
add_library(lvis STATIC src/lvis.cpp src/sceneLoader.cpp src/mesh.cpp src/camera.cpp src/lightPath.cpp src/pathVertex.cpp src/sphereSelect.cpp src/virtCam.cpp)

########################################################
# Linking
#########################################################
target_link_libraries(lvis -isystem ${CMAKE_SOURCE_DIR}/libAll/libGLU.so ${CMAKE_SOURCE_DIR}/libAll/libGL.so ${CMAKE_SOURCE_DIR}/libAll/libSM.so ${CMAKE_SOURCE_DIR}/libAll/libICE.so ${CMAKE_SOURCE_DIR}/libAll/libX11.so ${CMAKE_SOURCE_DIR}/libAll/libXext.so ${CMAKE_SOURCE_DIR}/libAll/libGLEW.so ${CMAKE_SOURCE_DIR}/libAll/libIL.so ${CMAKE_SOURCE_DIR}/libAll/libILU.so ${CMAKE_SOURCE_DIR}/libAll/libILUT.so assimp ${CMAKE_SOURCE_DIR}/libAll/libsfml-system.so ${CMAKE_SOURCE_DIR}/libAll/libsfml-window.so ${CMAKE_SOURCE_DIR}/libAll/libsfml-graphics.so ${CMAKE_SOURCE_DIR}/libAll/libsfml-audio.so )