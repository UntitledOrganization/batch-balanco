cmake_minimum_required(VERSION 3.0.0)
project(
    batch-balanco
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A Simple 2D Spritebatch Library")

set(LIBRARY_OUTPUT_DIRECTORY bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(OpenGL_GL_PREFERENCE GLVND)

# Find dependencies
find_package(GLEW CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_path(STB_INCLUDE_DIR NAMES stb_image.h)

include_directories(include ${STB_INCLUDE_DIR})
file(GLOB SOURCE_FILES src/*.cpp)

add_library(batch-balanco SHARED ${SOURCE_FILES})
target_link_libraries(batch-balanco PUBLIC GLEW::GLEW ${OPENGL_LIBRARIES})
target_include_directories(batch-balanco PUBLIC include)
