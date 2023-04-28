cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 14)

project(Project3)

set(SFML_DIR "<insert SFML library path")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(Project3 src/main.cpp src/ImGui-SFML/imgui-SFML.cpp src/ImGui/imgui.cpp src/ImGui/imgui_demo.cpp src/ImGui/imgui_draw.cpp src/ImGui/imgui_tables.cpp src/ImGui/imgui_widgets.cpp)

find_package(OpenGL REQUIRED)
add_subdirectory(3rdparty/RapidFuzz)

include_directories(${OPENGL_INCLUDE_DIR})

target_link_libraries(Project3 ${OPENGL_LIBRARIES})

target_link_libraries(Project3 sfml-graphics sfml-audio)
target_link_libraries(Project3 rapidfuzz::rapidfuzz)
