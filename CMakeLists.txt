cmake_minimum_required(VERSION 3.13)
project(EvoC2D)
include(ExternalProject)

set (CMAKE_CXX_STANDARD 14)
set(EXE_NAME evoc2d)
set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

find_package(OpenGL REQUIRED) #should be installed on machines with visual studio

#download dependencies

ExternalProject_Add(cglmProj
    GIT_REPOSITORY https://github.com/recp/cglm
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

ExternalProject_Add(freetypeProj
    GIT_REPOSITORY https://github.com/freetype/freetype
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

ExternalProject_Add(glfwProj
    GIT_REPOSITORY https://github.com/glfw/glfw
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION}
)

file(GLOB SHADERS ${CMAKE_SOURCE_DIR}/GLSLShaders/*.vsh
 ${CMAKE_SOURCE_DIR}/GLSLShaders/*.fsh)

foreach(SHADERFILE ${SHADERS})
configure_file(${SHADERFILE} ${CMAKE_BINARY_DIR} COPYONLY)
endforeach()

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/Creature/*.c
 ${CMAKE_SOURCE_DIR}/Genome/*.c
 ${CMAKE_SOURCE_DIR}/Graphics/*.c
 ${CMAKE_SOURCE_DIR}/Innovation/*.c
 ${CMAKE_SOURCE_DIR}/Environment/*.c
 ${CMAKE_SOURCE_DIR}/Simulation/*.c
 ${CMAKE_SOURCE_DIR}/GLAD/src/*.c)

file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/Creature/*.h
 ${CMAKE_SOURCE_DIR}/Genome/*.h
 ${CMAKE_SOURCE_DIR}/Graphics/*.h
 ${CMAKE_SOURCE_DIR}/Innovation/*.h
 ${CMAKE_SOURCE_DIR}/Environment/*.h
 ${CMAKE_SOURCE_DIR}/Simulation/*.h)

link_directories(${EXTERNAL_INSTALL_LOCATION}/lib)

add_executable(${EXE_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${EXE_NAME} PRIVATE 
 ${CMAKE_SOURCE_DIR}/Creature/
 ${CMAKE_SOURCE_DIR}/Genome/
 ${CMAKE_SOURCE_DIR}/Graphics/
 ${CMAKE_SOURCE_DIR}/Innovation/
 ${CMAKE_SOURCE_DIR}/Environment/
 ${CMAKE_SOURCE_DIR}/Simulation/
 ${CMAKE_SOURCE_DIR}/GLAD/include/
 ${EXTERNAL_INSTALL_LOCATION}/include/
 ${EXTERNAL_INSTALL_LOCATION}/include/freetype2)

add_dependencies(${EXE_NAME} cglmProj glfwProj freetypeProj)
target_link_libraries(${EXE_NAME} PRIVATE opengl32 glfw3 cglm freetyped)
