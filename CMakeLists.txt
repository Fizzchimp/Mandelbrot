# Specify cmake version minimum
cmake_minimum_required(VERSION 3.10.0)

# Specify project details
project(Mandelbrot
    VERSION 1.0.0
    DESCRIPTION "A simple Mandelbrot set renderer written in C++"
    LANGUAGES CXX
)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

### Maths
# Set the maths directory
set(MATHS_DIR /${CMAKE_SOURCE_DIR}/../dependencies/maths)

# Maths source files
set(MATHS_SOURCES
    ${MATHS_DIR}/src/vec2.cpp
    ${MATHS_DIR}/src/mat2.cpp
)


### ImGui
# Set the ImGui directory
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/../dependencies/imgui)

# ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
)

# Create the exectuable target and link all source files
add_executable(${PROJECT_NAME}
    src/Mandelbrot.cpp
    src/ContextManager.cpp
    src/Errors.cpp
    src/Renderer.cpp
    src/Shader.cpp
    ${IMGUI_SOURCES}
    ${MATHS_SOURCES}
)

# Copy resource files to build directory
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/res
    ${PROJECT_BINARY_DIR}/res
    COMMENT "Copying resources into binary directory ${PROJECT_BINARY_DIR}/res"
)
# Add the dependancy for copied resources
add_dependencies(${PROJECT_NAME} copy_resources)


# Specify include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${MATHS_DIR}/include
    
)

# Specify to link required libraries
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    GLEW::GLEW
    glfw
)