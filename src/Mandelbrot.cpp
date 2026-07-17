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
#include "SetAttributes.h"

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

        // Setting the mouse button callback function
        glfwSetMouseButtonCallback(window, mouseButton);

        // Setting the scroll wheel callback function
        glfwSetScrollCallback(window, scrollWheel);

        // Setting the window size callback function
        glfwSetWindowSizeCallback(window, resizeWindow);

        // Setting window maximise callback function
        glfwSetWindowMaximizeCallback(window, maximiseWindow);
    }

    void runEngine()
    {
        while (!glfwWindowShouldClose(window))
        {
            // Poll for and process events
            glfwPollEvents();

            // Get the position of the mouse on the complex plane
            mandelbrotAttribs.complexPlanePos = windowToComplex(mandelbrotAttribs, mousePos);

            // Prepare renderer for the next frame render
            renderer.newFrame();

            // Draw the mandelbrot set
            renderer.drawMandelbrotSet(mandelbrotAttribs, renderJuliaSet);


            if (renderJuliaSet)
            {
                // Render the mandelbrot set marker
                renderer.drawMandelbrotMarker(complexToUV(mandelbrotAttribs, markerPosition), markerRadius / largestWindowDim * 2); // TODO: Maybe change the way that largestWindowDim works.

                // Render the Julia Set
                renderer.drawJuliaSet(juliaAttribs, markerPosition);
            }


            // Draw the ImGui settings for the mandelbrot set
            renderer.drawMandelbrotSettings(mandelbrotAttribs, renderJuliaSet);
            
            // Draw the FPS counter
            renderer.drawFPS(width);
            
            // Render all new drawn elements
            renderer.render();
            std::cout << complexToUV(mandelbrotAttribs, vec2(0.0f, 0.0f)).x << "\n";
        }
    }

private:
    ContextManager contextManager; // Object to initialise GLFW and GLEW before anything else

    GLFWwindow* window = contextManager.window; // GLFW window pointer

    int& width = contextManager.width, height = contextManager.height; // Reference to the window dimensions (owned by contextManager) (window coordinates)
    float largestWindowDim = std::max(width, height); // Largest window dimension

    Renderer renderer; // Renderer object

	vec2 mousePos; // Position of the mouse on the screen (window coordinates)
    
    SetAttributes mandelbrotAttribs = SetAttributes(50, 50.0f, 2.0f, 3.0f, vec2(-0.5f, 0.0f), left); // Attributes for rendering the mandelbrot set
    SetAttributes juliaAttribs = SetAttributes(50, 50.0f, 2.0f, 6.0f, vec2(0.0f, 0.0f), right); // Attributes for rendering the julia set

    bool renderJuliaSet = false; // Signals if the julia set should be rendered

    vec2 markerPosition = vec2(); // The position of the marker on the complex plane (complex coordinates)
    bool markerSelected = false; // Flags if the marker is currently being held
    float markerRadius = 10.0f; // Radius of the marker in pixels (window coordinates)

    //// Display maintenance

    // Converts window coordinates to complex coordinates (window -> complex)
    vec2 windowToComplex(SetAttributes& set, vec2 windowPos)
    {
        if (renderJuliaSet && ((set.side == -1 && windowPos.x > width / 2) || (set.side == 1 && windowPos.x < width / 2))) {return vec2(NAN, NAN);}  // If both sets are being rendered, the screen is split so the sets window coordinates are different
        return vec2((windowPos.x - width / 2 - (width * renderJuliaSet / 4 * set.side)), (height / 2 - windowPos.y)) / largestWindowDim * 2.0f * set.zoom + set.center;
    }

    // Converts complex coordinates to window coordinates (complex -> window) // TODO: I dont think this works when height > width
    vec2 complexToWindow(SetAttributes& set, vec2 complexPos)
    {
        vec2 temp = (complexPos - set.center) / set.zoom / 2 * largestWindowDim;
        return vec2(temp.x + (width * renderJuliaSet / 4 * set.side) + width / 2, -temp.y + height / 2);
    }

    // Converts complex plane coordinates to UV coordinates (complex -> UV)
    vec2 complexToUV(SetAttributes& set, vec2 complexPos)
    {
        return (complexPos - set.center) / set.zoom + 0.5 * set.side * renderJuliaSet;
    }


    // Detects if the mouse is hovering over the marker (mousePos in window coordinates)
    bool isHoveringOnMarker() 
    {
        vec2 markerWindowPos = complexToWindow(mandelbrotAttribs, markerPosition);
        return ((markerWindowPos - mousePos).magnitude() < markerRadius);
    }

    // Updates set properties when zooming in or out (called on scrollwheel callback) // TODO: SHOULD THIS BE A SetAttribute METHOD?
    void changeSetZoom(SetAttributes& set, double zoom)
    {
        set.zoom *= std::pow(1.1, zoom);

        // Increase the maximum number of iterations used in shader calculation
        set.maxIterations = int(50 * std::log2(1.0f / set.zoom));
        if (set.maxIterations < 50)
            set.maxIterations = 50; // Ensure a minimum number of iterations

        // Increase the brightness value used in the shader calculation
        set.brightness = int(50 * std::log2(1.0f / set.zoom));
        if (set.brightness < 50.0f)
            set.brightness = 50.0f; // Ensure a minimum brightness value

        set.enforceMaxZoom();
        set.enforceSetBoundaries(width, height, renderJuliaSet);
    }

    // Updates the center set property (called on mouse movement callback) // TODO: SHOULD THIS BE A SetAttribute METHOD?
    void changeSetCenter(SetAttributes& set, vec2 windowPosOffset)
    {
        set.center = set.center + windowPosOffset * 2.0f * set.zoom / largestWindowDim;
        
        set.enforceMaxZoom();
        set.enforceSetBoundaries(width, height, renderJuliaSet);
    }


    ////// IO Callbacks

    // Mouse movement callback function
    static void mouseMovement(GLFWwindow* window, double xpos, double ypos) // TODO: Uses a coordinate conversion, make a method specifically for that conversion?
    {
		PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent((float)xpos, (float)ypos);

        if (engine->renderJuliaSet && (engine->markerSelected || (!io.WantCaptureMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)))
        {
            engine->markerPosition = engine->windowToComplex(engine->mandelbrotAttribs, vec2(xpos, ypos));
        }
        else if (!io.WantCaptureMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (!engine->renderJuliaSet || ((engine->mousePos.x - engine->width / 2) / engine->mandelbrotAttribs.side > 0))
            {
                engine->changeSetCenter(engine->mandelbrotAttribs, vec2(engine->mousePos.x - xpos, ypos - engine->mousePos.y));
            }
            else
            {
                engine->changeSetCenter(engine->juliaAttribs, vec2(engine->mousePos.x - xpos, ypos - engine->mousePos.y));
            }
        }

        engine->mousePos.x = xpos;
        engine->mousePos.y = ypos;
    }

    // Mouse button callback function
    static void mouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseButtonEvent(button, action);

        if (engine->renderJuliaSet)
        {
            if (button == GLFW_MOUSE_BUTTON_LEFT && engine->isHoveringOnMarker())
            {
                engine->markerSelected = action;
            }
            else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
            {
                engine->markerPosition = engine->windowToComplex(engine->mandelbrotAttribs, engine->mousePos);
            }
        }
    }

    // Scroll wheel callback function
    static void scrollWheel(GLFWwindow* window, double xoffset, double yoffset)
    {
		PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);

        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
        if (!io.WantCaptureMouse)
        {
            if (!engine->renderJuliaSet || ((engine->mousePos.x - engine->width / 2) / engine->mandelbrotAttribs.side > 0))
            {
                engine->changeSetZoom(engine->mandelbrotAttribs, -yoffset);
            }
            else
            {
                engine->changeSetZoom(engine->juliaAttribs, -yoffset);
            }
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
        engine->largestWindowDim = std::max(engine->width, engine->height);
        engine->mandelbrotAttribs.enforceSetBoundaries(engine->width, engine->height, engine->renderJuliaSet);
    }

    // Maximise window callback function
    static void maximiseWindow(GLFWwindow* window, int maximised)
    {
        PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);
        glfwGetWindowSize(window, &engine->width, &engine->height);
        engine->renderer.setProjection(engine->width, engine->height);
        engine->largestWindowDim = std::max(engine->width, engine->height);
    }
};


int main()
{
    PrettyEngine engine(1800, 900);
    engine.runEngine();
    return 0;
}