#include "Renderer.h"

Renderer::Renderer(GLFWwindow* window, int width, int height)
: window(window), mandelbrotShader(Shader("res/vertex.glsl", "res/mandelfrag.glsl"))
{
	// Bind the Shader Object
	mandelbrotShader.bind();

	// Initialise ImGui
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	// Imgui styling
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 3.0f;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;

	GLCALL(glGenVertexArrays(1, &vertexArrayID));
	GLCALL(glBindVertexArray(vertexArrayID));

	GLCALL(glGenBuffers(1, &vertexBufferID));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW));

	GLCALL(glEnableVertexAttribArray(0))
	GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr));

	// Sets the projection matrix based on the window dimensions
	setProjection(width, height);
}

Renderer::~Renderer()
{
	// Cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	GLCALL(glDeleteVertexArrays(1, &vertexArrayID));
	GLCALL(glDeleteBuffers(1, &vertexBufferID));
	GLCALL(glBindVertexArray(0));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

// Sets the projection matrix based on the width and the height of the window
void Renderer::setProjection(int width, int height)
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

// Prepare the renderer for drawing elements
void Renderer::newFrame()
{
	// Clear the screen
	GLCALL(glClear(GL_COLOR_BUFFER_BIT));

	// Set new ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

// Render the new elements on screen
void Renderer::render() // Not sure about passing the window pointer every time. Maybe set up window pointer as a class attribute?
{
	// Render all ImGui elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap front and back buffers
	glfwSwapBuffers(window);
}

// Draw the mandelbrot set
void Renderer::drawMandelbrotSet(int maxIterations, float brightness, float zoom, vec2 center)
{
	// Bind the mandelbrot shader
	mandelbrotShader.bind();

	// Set shader uniforms
	mandelbrotShader.setUniform1i("u_maxIterations", maxIterations);
	mandelbrotShader.setUniform1f("u_brightness", brightness);
	mandelbrotShader.setUniform1f("u_zoom", zoom);
	mandelbrotShader.setUniformMat2f("u_MVP", projection);
	mandelbrotShader.setUniformVec2f("u_center", center);

	// Draw the screen
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}

// Draw various settings
void Renderer::drawSettings(int& maxIterations, float& brightness, float& zoom, vec2& complexPlanePos, vec2& center)
{
	// ImGui::ShowDemoWindow();
            
            ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
            ImGui::SliderInt(" Max Iterations", &maxIterations, 50, 1000);
            ImGui::SliderFloat("Brightness", &brightness, 50.0f, 1000.0f);
            ImGui::Text("Zoom: %.4f", zoom);
            std::string posFormat = " %." + std::to_string(int(std::log10(1.0f / zoom)) + 3) + "f"; // Update the format based on zoom
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
}

// Draw the FPS counter
void Renderer::drawFPS(int width)
{
	ImGui::SetNextWindowPos(ImVec2(width - 90, 5), ImGuiCond_Always);
            ImGui::Begin("FPS", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::End();
}