cmake_minimum_required(VERSION 2.8)

#########################################################
# GENERAL INCLUDE
#########################################################
include_directories("${CMAKE_SOURCE_DIR}/lvis/include")
#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)
#########################################################
# FIND GLEW
#########################################################
find_package(GLEW REQUIRED)
if(GLEW_FOUND)
  include_directories(${GLEW_INCLUDE_DIR})
  link_directories(${GLEW_LIBRARIES})
endif(GLEW_FOUND)
if(NOT GLEW_FOUND)
    message(ERROR "GLEW not found!")
endif(NOT GLEW_FOUND)
#########################################################
# FIND SFML
#########################################################
find_package(SFML 2 COMPONENTS system window graphics audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})
if(NOT SFML_FOUND)
    message(ERROR "SFML not found!")
endif(NOT SFML_FOUND)
#########################################################
# FIND DevIL
#########################################################
find_package(DevIL REQUIRED)
if(IL_FOUND)
  include_directories(${IL_INCLUDE_DIR})
  link_directories(${IL_LIBRARIES})
  link_directories(${ILU_LIBRARIES})
  link_directories(${ILUT_LIBRARIES})
endif(IL_FOUND)
if(NOT IL_FOUND)
    message(ERROR "DevIL not found!")
endif(NOT IL_FOUND)
#########################################################
# FIND ASSIMP
#########################################################
Find_package ("Boost" REQUIRED)
link_directories (${Boost_LIBRARY_DIRS})
include_directories (${Boost_INCLUDE_DIRS})
find_package(ASSIMP REQUIRED)
if(ASSIMP_FOUND)
  include_directories(${ASSIMP_INCLUDE_DIR})
  link_directories(${ASSIMP_LIBRARIES})
  add_definitions(${OpenGL_DEFINITIONS})
endif(ASSIMP_FOUND)
if(NOT ASSIMP_FOUND)
    message(ERROR "ASSIMP not found!")
endif(NOT ASSIMP_FOUND)

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
target_link_libraries(lvis -isystem ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${IL_LIBRARIES} ${ILU_LIBRARIES} ${ILUT_LIBRARIES} ${Boost_LIBRARY} ${ASSIMP_LIBRARIES} ${SFML_LIBRARIES} )