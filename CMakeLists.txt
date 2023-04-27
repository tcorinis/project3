cmake_minimum_required(VERSION 3.23)
project(Project3)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(3rdparty/RapidFuzz)
add_executable(Project3 src/main.cpp src/LyricMatcher.h src/Lyric.h src/wordConverter.h )

target_link_libraries(Project3 rapidfuzz::rapidfuzz)