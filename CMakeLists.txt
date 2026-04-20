cmake_minimum_required(VERSION 3.10)
set(OpenGL_GL_PREFERENCE GLVND)
project(OpenGLGame)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Tell CMake where to look for your .h files
include_directories(
    include 
    external/imgui 
    external/imgui/backends
    ${GLFW_INCLUDE_DIRS}
)

# 3. Find necessary Linux libraries
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 

# 4. Define your executable and its source files
add_executable(Game 
    src/main.cpp 
    src/glad.c
    src/stb_image.cpp
    src/Shader.cpp
    src/Camera.cpp
    src/Physics.cpp
    src/Game.cpp
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# 5. Link the libraries to your game
target_link_libraries(Game 
    ${OPENGL_LIBRARIES} 
    ${GLFW_LIBRARIES} 
    dl # Necessary for GLAD on Linux
)

# 6. CRITICAL: Tell ImGui to use GLAD
target_compile_definitions(Game PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)