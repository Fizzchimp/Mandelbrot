#include "Renderer.h"

Renderer::Renderer(GLFWwindow* window, int width, int height)
: window(window), mandelbrotShader(Shader("res/vertex.glsl", "res/mandelfrag.glsl")), juliaShader(Shader("res/vertex.glsl", "res/juliafrag.glsl")), markerShader(Shader("res/markervert.glsl", "res/markerfrag.glsl"))
{
	// Bind the Shader Object
	// markerShader.bind();
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

	// Generate vertex array
	GLCALL(glGenVertexArrays(1, &vertexArrayID));
	GLCALL(glBindVertexArray(vertexArrayID));

	// Generate vertex buffer
	GLCALL(glGenBuffers(1, &vertexBufferID));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertices, GL_STATIC_DRAW));

	// Set vertex array attributes
	GLCALL(glEnableVertexAttribArray(0))
	GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

	// Sets the projection matrix based on the window dimensions
	setProjection(width, height);

	// Enable blending
	GLCALL(glEnable(GL_BLEND));
	GLCALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
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
		projection.setValue(0, 0, 1.0f);
		projection.setValue(1, 1, (float)height / width);
	}
	else
	{
		projection.setValue(0, 0, (float)width / height);
		projection.setValue(1, 1, 1.0f);
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
void Renderer::render()
{
	// Render all ImGui elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Swap front and back buffers
	glfwSwapBuffers(window);
}

// Draw the mandelbrot set
void Renderer::drawMandelbrotSet(SetAttributes attributes, bool renderJuliaSet)
{
	// Bind the mandelbrot shader
	mandelbrotShader.bind();

	// Set shader uniforms // TODO: Do these need to be set at each render? Or can they be set once and updated when needed?
	mandelbrotShader.setUniform1i("u_maxIterations", attributes.maxIterations);
	mandelbrotShader.setUniform1f("u_brightness", attributes.brightness);
	mandelbrotShader.setUniform1f("u_zoom", attributes.zoom);
	mandelbrotShader.setUniformVec2f("u_center", attributes.center);

	if (renderJuliaSet)
	{
		translation.setValue(0, 2, attributes.side);
		mandelbrotShader.setUniformMat3f("u_MVP", projection * stretch);
		mandelbrotShader.setUniformMat3f("u_transform", stretch * translation);
	} else {
		mandelbrotShader.setUniformMat3f("u_MVP", projection);
		mandelbrotShader.setUniformMat3f("u_transform", mat3());
	}

	// Draw the screen
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}

// Draw the julia set
void Renderer::drawJuliaSet(SetAttributes attributes, vec2 startingPos)
{
	// Bind the julia shader
	juliaShader.bind();

	// Set shader uniforms
	juliaShader.setUniform1i("u_maxIterations", attributes.maxIterations);
	juliaShader.setUniform1f("u_brightness", attributes.brightness);
	juliaShader.setUniform1f("u_zoom", attributes.zoom);
	juliaShader.setUniformVec2f("u_center", attributes.center);
	juliaShader.setUniformVec2f("u_startingPos", startingPos);
	juliaShader.setUniformMat3f("u_MVP", projection * stretch);

	translation.setValue(0, 2, attributes.side);
	juliaShader.setUniformMat3f("u_transform", stretch * translation);

	// Draw the screen
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}

// Draw a marker on the mandelbrot set determining how the julia set is drawn
void Renderer::drawMandelbrotMarker(vec2 position, float radius)
{
	mat3 translation = mat3(
		1.0f, 0.0f, position.x,
		0.0f, 1.0f, position.y,
		0.0f, 0.0f, 1.0f
	);

	mat3 scale = {
		radius, 0.0f,   0.0f,
		0.0f,   radius, 0.0f,
		0.0f,   0.0f,   1.0f
	};
	// Bind the shader
	markerShader.bind();

	// Set shader uniforms
	markerShader.setUniformMat3f("u_projection", projection);
	markerShader.setUniformMat3f("u_transform", translation * scale);
	markerShader.setUniformMat3f("u_translation", translation);

	markerShader.setUniformVec2f("u_markerCenter", position);

	// Draw the marker
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}

// Draw the ImGui settings
void Renderer::drawMandelbrotSettings(SetAttributes& mandelbrotAttribs, bool& renderJuliaSet)
{
	// ImGui::ShowDemoWindow();
            
            ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
            ImGui::SliderInt(" Max Iterations", &mandelbrotAttribs.maxIterations, 50, 1000);
            ImGui::SliderFloat("Brightness", &mandelbrotAttribs.brightness, 50.0f, 1000.0f);
            ImGui::Text("Zoom: %.4f", mandelbrotAttribs.zoom);
            std::string posFormat = " %." + std::to_string(int(std::log10(1.0f / mandelbrotAttribs.zoom)) + 3) + "f"; // Update the format based on zoom
            ImGui::Text(("Mouse Position:" + posFormat + posFormat).c_str(), mandelbrotAttribs.complexPlanePos.x, mandelbrotAttribs.complexPlanePos.y);
            if (ImGui::Button("Reset View"))
            {
                mandelbrotAttribs.center.x = -0.5f;
                mandelbrotAttribs.center.y =  0.0f;
                mandelbrotAttribs.zoom = 2.0f;
                mandelbrotAttribs.maxIterations = 50;
                mandelbrotAttribs.brightness = 50.0f;
            }
			if (ImGui::Checkbox("Show Julia Set: ", &renderJuliaSet))
			{
				float maxZoom = 3.0f + 3.0f * renderJuliaSet;
        		if (mandelbrotAttribs.zoom > maxZoom) { mandelbrotAttribs.zoom = maxZoom; }
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