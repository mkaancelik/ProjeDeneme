cmake_minimum_required(VERSION 3.10)
project(VirtualMuseum)

set(CMAKE_CXX_STANDARD 11)

include_directories(Libraries/include)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

# Source files
set(SOURCE_FILES
    Main.cpp
    Shader.cpp
    Mesh.cpp
    Camera.cpp
    glad.c
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
)

set_source_files_properties(glad.c PROPERTIES LANGUAGE C)

add_executable(VirtualMuseum ${SOURCE_FILES})

# Link libraries
target_link_libraries(VirtualMuseum
    Libraries/lib/glfw3.lib
    opengl32
    # Add other libraries as needed
)

# ImGui integration (copying necessary files)
file(GLOB IMGUI_SOURCES "Libraries/include/imgui-1.91.9b/*.cpp")
file(GLOB IMGUI_HEADERS "Libraries/include/imgui-1.91.9b/*.h")
file(GLOB IMGUI_BACKEND_SOURCES "Libraries/include/imgui-1.91.9b/backends/*.cpp")
file(GLOB IMGUI_BACKEND_HEADERS "Libraries/include/imgui-1.91.9b/backends/*.h")

source_group(TREE Libraries/include/imgui-1.91.9b/ FILES ${IMGUI_SOURCES} ${IMGUI_HEADERS} ${IMGUI_BACKEND_SOURCES} ${IMGUI_BACKEND_HEADERS})
