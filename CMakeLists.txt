cmake_minimum_required(VERSION 3.23)
project(Project3)

set(CMAKE_CXX_STANDARD 14)

# replace <SFML library path> with path of SFML
set(SFML_DIR "<insert SFML library path>")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_subdirectory(3rdparty/RapidFuzz)
add_executable(Project3 src/main.cpp src/LyricMatcher.h src/Lyric.h src/wordConverter.h)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

target_link_libraries(Project3 sfml-graphics sfml-audio)
target_link_libraries(Project3 rapidfuzz::rapidfuzz)
