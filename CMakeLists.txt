cmake_minimum_required(VERSION 3.10)
project(Engine)

find_package(OpenGL REQUIRED)

find_package(glfw3 QUIET)

if(NOT glfw3_FOUND)
    message(STATUS "GLFW not found in the system - build from libraries")
    add_subdirectory(libraries/glfw)
    set(GLFW_LIB glfw)
else()
    message(STATUS "Using the system GLFW")
    set(GLFW_LIB glfw)
endif()

find_package(glm QUIET)

if(NOT glm_FOUND)
    message(STATUS "GLM not found in the system - build from libraries")
    add_subdirectory(libraries/glm)
    set(GLM_LIB glm)
else()
    message(STATUS "Using the system GLM")
    set(GLM_LIB glm)
endif()

find_path(GLI_INCLUDE_DIR gli/gli.hpp)

if(NOT GLI_INCLUDE_DIR)
    message(STATUS "GLI not found in the system - build from libraries")
    set(GLI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libraries/gli)
endif()

add_library(gli INTERFACE)
target_include_directories(gli INTERFACE ${GLI_INCLUDE_DIR})
set(GLI_LIB gli)

file(GLOB SOURCES "source/*.cpp")

add_executable(Engine
    ${SOURCES}
    libraries/glad/glad.c
)

target_include_directories(Engine PRIVATE
    libraries
    include
)

target_link_libraries(Engine PRIVATE
    glfw
    ${GLFW_LIB}
    ${GLM_LIB}
    ${GLI_LIB}
    OpenGL::GL
)
