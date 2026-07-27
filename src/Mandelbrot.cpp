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
		: width(inputWidth), height(inputHeight), contextManager(width, height), renderer(Renderer(window, width, height))
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
        // TODO: Make an fps cap option.
        while (!glfwWindowShouldClose(window))
        {
            // Poll for and process events
            glfwPollEvents();

            // Get the position of the mouse on the complex plane
            mandelbrotAttribs.complexPlanePos = windowToComplex(mandelbrotAttribs, mousePos);

            // Prepare renderer for the next frame render
            renderer.newFrame();

            // Render the mandelbrot set
            renderer.drawMandelbrotSet(mandelbrotAttribs, renderJuliaSet);

            // Draw the grid for mandelbrot set
            if (renderGrid)
            {
                double diff = getGridDiff(mandelbrotAttribs);
                renderer.drawGrid(mandelbrotAttribs, width, height, complexToWindowd(mandelbrotAttribs, vec2d(0.0f, 0.0f)), complexToWindowd(mandelbrotAttribs, vec2d(diff, diff)).x, renderJuliaSet);
            }

            // Julia set and marker
            if (renderJuliaSet)
            {
                // Render the mandelbrot set marker
                renderer.drawMandelbrotMarker(complexToUV(mandelbrotAttribs, markerPos), markerRadius / std::min(width, height) * 2);


                // Render ImGui settings for the marker
                renderer.drawMarkerSettings(markerPos);

                // Render the Julia Set
                renderer.drawJuliaSet(juliaAttribs, vec2(markerPos.x, markerPos.y));

                // Draw the grid for julia set
                if (renderGrid)
                {
                    double diff = getGridDiff(juliaAttribs);
                    renderer.drawGrid(juliaAttribs, width, height, complexToWindowd(juliaAttribs, vec2d(0.0f, 0.0f)), complexToWindowd(juliaAttribs, vec2d(diff, diff)).x, renderJuliaSet);
                }
            }

            // Render the ImGui settings for the mandelbrot set
            renderer.drawMandelbrotSettings(mandelbrotAttribs, renderJuliaSet, renderGrid);
            
            // Render the FPS counter
            renderer.drawFPS(width);
            
            // Render all new drawn elements
            renderer.render();
        }
    }

private:
    int width, height; // Reference to the window dimensions (window coordinates)

    ContextManager contextManager; // Object to initialise GLFW and GLEW before anything else

    GLFWwindow* window = contextManager.window; // GLFW window pointer


    Renderer renderer; // Renderer object

	vec2 mousePos; // Position of the mouse on the screen (window coordinates)
    
    SetAttributes mandelbrotAttribs = SetAttributes(50, 50.0f, 2.0f, 4.0f, vec2d(-0.5f, 0.0f), left); // Attributes for rendering the mandelbrot set
    SetAttributes juliaAttribs = SetAttributes(50, 50.0f, 2.0f, 6.0f, vec2d(0.0f, 0.0f), right); // Attributes for rendering the julia set

    bool renderJuliaSet = false; // Signals if the julia set should be rendered
    bool renderGrid = false; // Signals if the grid should be rendered

    vec2d markerPos = vec2d(); // The position of the marker on the complex plane (complex coordinates)
    bool markerSelected = false; // Flags if the marker is currently being held
    float markerRadius = 10.0f; // Radius of the marker in pixels (window coordinates)

    //// Display maintenance

    // Converts window coordinates to complex coordinates (window -> complex)
    vec2d windowToComplex(SetAttributes& set, vec2 windowPos)
    {
        int thing = std::min(width, height);

        if (renderJuliaSet && ((set.side == -1 && windowPos.x > width / 2) || (set.side == 1 && windowPos.x < width / 2))) {return vec2d(NAN, NAN);}  // If both sets are being rendered, the screen is split so the sets window coordinates are different
        return vec2d((windowPos.x - width / 2.0f - (width * renderJuliaSet / 4.0f * set.side)), (height / 2.0f - windowPos.y)) / std::min(width, height) * 2.0f * set.zoom + set.center;
    }

    // Converts complex coordinates to window coordinates (complex -> window)
    vec2 complexToWindow(SetAttributes& set, vec2d complexPos)
    {
        vec2d temp = (complexPos - set.center) / set.zoom * std::min(width, height);
        vec2d temp2 = vec2d(temp.x + (width * renderJuliaSet / 2 * set.side) + width, -temp.y + height) / 2.0f;
        // return vec2(temp.x + (width * renderJuliaSet / 2 * set.side) + width, -temp.y + height) / 2.0f;
        return vec2(temp2.x, temp2.y);
    }

    // Converts complex coordinates to window coordinates MORE DOUBLE? (complex -> window) // TODO: IS THIS NECESSARY?
    vec2d complexToWindowd(SetAttributes& set, vec2d complexPos)
    {
        vec2d temp = (complexPos - set.center) / set.zoom * std::min(width, height);
        vec2d temp2 = vec2d(temp.x + (width * renderJuliaSet / 2 * set.side) + width, -temp.y + height) / 2.0f;
        // return vec2(temp.x + (width * renderJuliaSet / 2 * set.side) + width, -temp.y + height) / 2.0f;
        return temp2;
    }

    // Converts complex plane coordinates to UV coordinates (complex -> UV)
    vec2 complexToUV(SetAttributes& set, vec2d complexPos)
    {
        vec2d temp = (complexPos - set.center) / set.zoom + vec2d(0.5f * (width > height ? (double)width / height : 1.0f) * set.side * renderJuliaSet, 0.0);
        return vec2(temp.x, temp.y);
    }


    // Detects if the mouse is hovering over the marker (mousePos in window coordinates)
    bool isHoveringOnMarker() 
    {
        vec2 markerWindowPos = complexToWindow(mandelbrotAttribs, markerPos);
        return ((markerWindowPos - vec2(mousePos.x, mousePos.y)).magnitude() < markerRadius);
    }

    // Calculates the distance two lines on the graph will be (window distance)
    double getGridDiff(SetAttributes set)
    {
        int exp = std::floor(std::log10(set.zoom));
        double mant = set.zoom / std::pow(10, (int)exp);

        double diff;

        if (1 < mant && mant <= 2) {diff = std::pow(10, exp);}
        else if (2 < mant && mant <= 5) {diff = 2 * std::pow(10, exp);}
        else {diff = 5 * std::pow(10, exp);}

        return diff;
    }



    ////// IO Callbacks

    // Mouse movement callback function // TODO: DOES WEIRD THINGS WHEN YOU HOLD DOWN MOUSE AND MOVE BETWEEN SETS ON SCREEN
    static void mouseMovement(GLFWwindow* window, double xpos, double ypos)
    {
		PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);
        ImGuiIO& io = ImGui::GetIO();
        io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
        io.AddMousePosEvent((float)xpos, (float)ypos);

        if (engine->renderJuliaSet && (engine->markerSelected || (!io.WantCaptureMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)))
        {
            engine->markerPos = engine->windowToComplex(engine->mandelbrotAttribs, vec2(xpos, ypos));
        }
        else if (!io.WantCaptureMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            if (!engine->renderJuliaSet || ((engine->mousePos.x - engine->width / 2) / engine->mandelbrotAttribs.side > 0))
            {
                engine->mandelbrotAttribs.updateCenter(vec2d(engine->mousePos.x - xpos, ypos - engine->mousePos.y), engine->width, engine->height, engine->renderJuliaSet);
            }
            else
            {
                engine->juliaAttribs.updateCenter(vec2d(engine->mousePos.x - xpos, ypos - engine->mousePos.y), engine->width, engine->height, engine->renderJuliaSet);
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
                engine->markerPos = engine->windowToComplex(engine->mandelbrotAttribs, engine->mousePos);
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
                engine->mandelbrotAttribs.updateZoom(-yoffset, engine->width, engine->height, engine->renderJuliaSet);
            }
            else
            {
                engine->juliaAttribs.updateZoom(-yoffset, engine->width, engine->height, engine->renderJuliaSet);
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
        engine->mandelbrotAttribs.enforceSetBoundaries(engine->width, engine->height, engine->renderJuliaSet);
    }

    // Maximise window callback function
    static void maximiseWindow(GLFWwindow* window, int maximised)
    {
        PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);
        glfwGetWindowSize(window, &engine->width, &engine->height);
        engine->renderer.setProjection(engine->width, engine->height);
    }
};


int main()
{
    PrettyEngine engine(1800, 900);
    engine.runEngine();
    return 0;
}