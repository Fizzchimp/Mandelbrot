#include <iostream>
#include <cassert>

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

// Used to initialise and create the opengl context and window
struct ContextManager
{   
    // Pointer to the GLFW window object
    GLFWwindow* window;

    // Window dimensions
    int width, height;
    
    // Constructor with the window dimensions
    ContextManager(int inputWidth, int inputHeight);

    ~ContextManager();
};

