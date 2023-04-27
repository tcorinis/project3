cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 14)

project(Project2)
set(SFML_DIR "D:/Libraries/SFML-2.5.1/lib/cmake/SFML")
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(Project2 main.cpp ImGui-SFML/imgui-SFML.cpp ImGui/imgui.cpp ImGui/imgui_demo.cpp ImGui/imgui_draw.cpp ImGui/imgui_tables.cpp ImGui/imgui_widgets.cpp)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(Project2 ${OPENGL_LIBRARIES})

target_link_libraries(Project2 sfml-graphics sfml-audio)