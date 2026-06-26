#include "ContextManager.h"

// Constructor with the window dimensions
ContextManager::ContextManager(int inputWidth, int inputHeight)
    : width(inputWidth), height(inputHeight)
{
    // Initialize the library
    if (!glfwInit())
    {
        std::cout << "Failed to initialise glfw\n";
        assert(false);
    }

    // Specify the glfw version required
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // Specify to use the core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(inputWidth, inputHeight, "Mandelbrot", nullptr, nullptr);

    // Check if window was created succesfully
    if (!window)
    {
        std::cout << "Failed to create glfw window\n";
    }

    // Set the width and height to the windows width and height
    glfwGetWindowSize(window, &width, &height);

    // Maximise the window after creation (Setting GLFW_MAXIMISED as a hint isnt supported)
    glfwMaximizeWindow(window);

    // Make the window's context current
    glfwMakeContextCurrent(window);


    // Initialise GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialise glew\n";
        assert(!GLEW_OK);
    }

    // Display the GLEW version to the console
    std::cout << glGetString(GL_VERSION) << "\n";
}

ContextManager::~ContextManager()
{
    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}