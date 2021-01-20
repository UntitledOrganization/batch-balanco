cmake_minimum_required(VERSION 3.0.0)
project(
    batch-balanco
    VERSION 0.0.1
    LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_CXX_STANDARD 17)

find_path(STB_INCLUDE_DIR NAMES stb_image.h)

include_directories(include ${STB_INCLUDE_DIR})
file(GLOB SOURCE_FILES src/*.cpp)

add_library(batch-balanco ${SOURCE_FILES})