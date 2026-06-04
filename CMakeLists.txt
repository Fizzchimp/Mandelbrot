cmake_minimum_required(VERSION 3.10.0)
project(Mandelbrot
    VERSION 0.1.0
    DESCRIPTION "A simple Mandelbrot set renderer written in C++"
    LANGUAGES CXX
)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(Mandelbrot
    src/Mandelbrot.cpp
    src/Errors.cpp
    src/Renderer.cpp
    src/Shader.cpp

    include/imgui/imgui_demo.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_impl_glfw.cpp
    include/imgui/imgui_impl_opengl3.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui.cpp
)

# Copy resource files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/res DESTINATION ${CMAKE_BINARY_DIR})

# Add compile definition for resource directory path
target_compile_definitions(Mandelbrot PRIVATE 
    RESOURCE_DIR="${CMAKE_BINARY_DIR}/res")


include_directories(${CMAKE_SOURCE_DIR}/include)

target_link_libraries(Mandelbrot
    OpenGL::GL
    GLEW::GLEW
    glfw
)