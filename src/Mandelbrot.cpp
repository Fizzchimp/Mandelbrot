#include <iostream>
#include <cmath>

#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ContextManager.h"
#include "Errors.h"
#include "Shader.h"
#include "Renderer.h"

#include "mat2.h"
#include "vec2.h"


class PrettyEngine
{
public:
    PrettyEngine(int inputWidth, int inputHeight)
		: contextManager(inputWidth, inputHeight), renderer(Renderer(window, width, height))
    {
        // Set a reference to this object in the window object
        glfwSetWindowUserPointer(window, this);

        // Setting the cursor callback function
        glfwSetCursorPosCallback(window, mouseMovement);
        // Setting the scroll wheel callback function
        glfwSetScrollCallback(window, scrollWheel);
        // Setting the window size callback function
        glfwSetWindowSizeCallback(window, resizeWindow);
    }

    void runEngine()
    {
        while (!glfwWindowShouldClose(window))
        {
            // Poll for and process events
            glfwPollEvents();

            // Get the position of the mouse on the complex plane
            getPositionOnComplexPlane();

            // Prepare renderer for the next frame render
            renderer.newFrame();

            // Draw the mandelbrot set
            renderer.drawMandelbrotSet(maxIterations, brightness, zoom, center);

            // Draw the ImGui settings
            renderer.drawSettings(maxIterations, brightness, zoom, complexPlanePos, center);
            
            // Draw the FPS counter
            renderer.drawFPS(width);
            
            // Render all new drawn elements
            renderer.render();
        }
    }

private:
    ContextManager contextManager; // Object to initialise GLFW and GLEW before anything else

    GLFWwindow* window = contextManager.window; // GLFW window pointer

    int& width = contextManager.width, height = contextManager.height; // Reference to the window dimensions (owned by contextManager)

    Renderer renderer; // Renderer object

	vec2 center = vec2(-0.5f, 0.0f); // The position of the center of the screen on the complex plane
	vec2 mousePos; // Position of the mouse on the screen
	vec2 complexPlanePos; // Position of the mouse on the complex plane

	float zoom = 2.0f; // Zoom level of the complex plane
    int maxIterations = 50; // Maximum number of iterations the fragment shader is allowed to calculate
    float brightness= 50.0f; // Brightness of the fractal (Changes based on the zoom)
    
    std::string posFormat = "%.4f"; // Imgui format for displaying complexPlanePos


    
    //// Display maintenance

    // Ensure the center is within the set boundaries
    void checkCenterBoundaries()
    {
        if (center.x * zoom < -2.5f) { center.x = -2.5f / zoom; }
        else if (center.x * zoom > 1.5f) { center.x = 1.5f / zoom; }
        if (center.y * zoom < -1.5f) { center.y = -1.5f / zoom; }
        else if (center.y * zoom > 1.5f) { center.y = 1.5f / zoom; }
    }

    // Ensure the zoom value is lower than the maximum zoom value
    void checkMaxZoom()
    {
        if (zoom > 3.0f) { zoom = 3.0f; }
    }



    //// IO Callbacks

    // Mouse movement callback function
    static void mouseMovement(GLFWwindow* window, double xpos, double ypos)
    {
		PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent((float)xpos, (float)ypos);

        if (!io.WantCaptureMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            engine->center.x -= (xpos - engine->mousePos.x) * 2.0f * engine->zoom / engine->width;
            engine->center.y += (ypos - engine->mousePos.y) * 2.0f * engine->zoom / engine->width;

            engine->checkCenterBoundaries();
            engine->checkMaxZoom();
        }

        engine->mousePos.x = xpos;
        engine->mousePos.y = ypos;
    }

    // Scroll wheel callback function
    static void scrollWheel(GLFWwindow* window, double xoffset, double yoffset)
    {
		PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);

        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
        if (!io.WantCaptureMouse)
        {
            engine->zoom *= std::pow(1.1, -yoffset);

            // Increase the maximum number of iterations used in shader calculation
            engine->maxIterations = int(50 * std::log2(1.0f / engine->zoom));
            if (engine->maxIterations < 50)
                engine->maxIterations = 50; // Ensure a minimum number of iterations

            // Increase the brightness value used in the shader calculation
            engine->brightness = int(50 * std::log2(1.0f / engine->zoom));
            if (engine->brightness < 50.0f)
                engine->brightness = 50.0f; // Ensure a minimum brightness value

            engine->checkCenterBoundaries();
            engine->checkMaxZoom();
        }
    }

    // Resize window callback function
    static void resizeWindow(GLFWwindow* window, int newWidth, int newHeight)
    {
		PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);

        engine->width = newWidth;
        engine->height = newHeight;
        glViewport(0, 0, engine->width, engine->height);
        engine->renderer.setProjection(engine->width, engine->height);
    }


    // Get the position of the mouse on the complex plane
    void getPositionOnComplexPlane()
    {
        complexPlanePos.x = (mousePos.x - width / 2) / width * 2 * zoom + center.x;
        complexPlanePos.y = (height / 2 - mousePos.y) / width * 2 * zoom + center.y;
    }
};


int main()
{
    PrettyEngine engine(1600, 900);
    engine.runEngine();
    return 0;
}