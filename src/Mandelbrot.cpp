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
		: contextManager(inputWidth, inputHeight), shader(Shader("res/vertex.glsl", "res/mandelfrag.glsl")), renderer(Renderer())
    {
        // Initialise ImGui
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 460 core");

        // Imgui styling
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 3.0f;
        style.FrameRounding = 3.0f;
        style.GrabRounding = 3.0f;


        // Sets the projection matrix based on the window dimensions
        setProjection(projection, width, height);

        // Bind the Shader Object
        shader.bind();

        // Set a reference to this object in the window
        glfwSetWindowUserPointer(window, this);


        // Setting the cursor callback function
        glfwSetCursorPosCallback(window, mouseMovement);
        // Setting the scroll wheel callback function
        glfwSetScrollCallback(window, scrollWheel);
        // Setting the window size callback function
        glfwSetWindowSizeCallback(window, resizeWindow);
    }

    ~PrettyEngine()
    {
        // Cleanup imgui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void runEngine()
    {
        while (!glfwWindowShouldClose(window))
        {
            // Poll for and process events
            glfwPollEvents();

            getPositionOnComplexPlane();

            shader.setUniform1i("u_maxIterations", maxIterations);
            shader.setUniform1f("u_brightness", brightness);
            shader.setUniform1f("u_zoom", zoom);
            shader.setUniformMat2f("u_MVP", projection);
            shader.setUniformVec2f("u_center", center);

            renderer.draw();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
            ImGui::SliderInt(" Max Iterations", &maxIterations, 50, 1000);
            ImGui::SliderFloat("Brightness", &brightness, 50.0f, 1000.0f);
            ImGui::Text("Zoom: %.4f", zoom);
            posFormat = " %." + std::to_string(int(std::log10(1.0f / zoom)) + 3) + "f"; // Update the format based on zoom
            ImGui::Text(("Position:" + posFormat + posFormat).c_str(), complexPlanePos.x, complexPlanePos.y);
            if (ImGui::Button("Reset View"))
            {
                center.x = -0.5f;
                center.y =  0.0f;
                zoom = 2.0f;
                maxIterations = 50;
                brightness = 50.0f;
            }
            ImGui::End();

            ImGui::SetNextWindowPos(ImVec2(width - 90, 5), ImGuiCond_Always);
            ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::End();


            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

private:
    ContextManager contextManager; // Object to initialise GLFW and GLEW before anything else

    GLFWwindow*& window = contextManager.window; // Reference to the GLFW window pointer (owned by contextManager)

    int& width = contextManager.width, height = contextManager.height; // Reference to the window dimensions (owned by contextManager)


    Shader shader; // Shader object
    Renderer renderer; // Renderer object

    mat2 projection; // Projection matrix for the shader (Based on the window dimensions)


	vec2 center = vec2(-0.5f, 0.0f); // The position of the center of the screen on the complex plane
	vec2 mousePos; // Position of the mouse on the screen
	vec2 complexPlanePos; // Position of the mouse on the complex plane

	float zoom = 2.0f; // Zoom level of the complex plane
    int maxIterations = 50; // Maximum number of iterations the fragment shader is allowed to calculate
    float brightness= 50.0f; // Brightness of the fractal (Changes based on the zoom)
    
    std::string posFormat = "%.4f"; // Imgui format for displaying complexPlanePos


    
    //// Display maintenance

    // Sets the projection matrix based on the width and the height of the window
    void setProjection(mat2& projection, int width, int height)
    {
        if (width > height)
        {
            projection.setValue(1, 1, (float)height / width);
        }
        else
        {
            projection.setValue(0, 0, (float)width / height);
        }
    }

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

    // Resize window callback function
    static void resizeWindow(GLFWwindow* window, int newWidth, int newHeight)
    {
		PrettyEngine* engine = (PrettyEngine*)glfwGetWindowUserPointer(window);


        engine->width = newWidth;
        engine->height = newHeight;
        glViewport(0, 0, engine->width, engine->height);
        engine->setProjection(engine->projection, engine->width, engine->height);
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