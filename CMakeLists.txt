cmake_minimum_required(VERSION 3.10)
set(OpenGL_GL_PREFERENCE GLVND)
project(OpenGLGame)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 1. Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. Tell CMake where to look for your .h files
include_directories(include)

# 3. Find necessary Linux libraries
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 

# 4. Define your executable and its source files
add_executable(MyGame 
    src/main.cpp 
    src/glad.c
    src/stb_image.cpp
    src/Shader.cpp
)

# 5. Link the libraries to your game
target_link_libraries(MyGame 
    ${OPENGL_LIBRARIES} 
    ${GLFW_LIBRARIES} 
    dl # Necessary for GLAD on Linux
)