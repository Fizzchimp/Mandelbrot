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

	// ImGui styling
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 3.0f;
	style.FrameRounding = 3.0f;
	style.GrabRounding = 3.0f;

	// // ImGui draw list init
	// draw_list = ImGui::GetWindowDrawList();

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
		projection.setValue(0, 0, (float)width / height);
		projection.setValue(1, 1, 1.0f);
	}
	else
	{
		projection.setValue(0, 0, 1.0f);
		projection.setValue(1, 1, (float)height / width);
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

	// Set shader uniforms
	mandelbrotShader.setUniform1i("u_maxIterations", attributes.maxIterations);
	mandelbrotShader.setUniform1f("u_brightness", attributes.brightness);
	mandelbrotShader.setUniform1f("u_zoom", attributes.zoom);
	mandelbrotShader.setUniformVec2f("u_center", vec2(attributes.center.x, attributes.center.y));

	if (renderJuliaSet)
	{
		translation.setValue(0, 2, attributes.side);
		mandelbrotShader.setUniformMat3f("u_projection", projection * stretch);
		mandelbrotShader.setUniformMat3f("u_transform", stretch * translation);
	} else {
		mandelbrotShader.setUniformMat3f("u_projection", projection);
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
	juliaShader.setUniformVec2f("u_center", vec2(attributes.center.x, attributes.center.y));
	juliaShader.setUniformVec2f("u_startingPos", startingPos);
	juliaShader.setUniformMat3f("u_projection", projection * stretch);

	translation.setValue(0, 2, attributes.side);
	juliaShader.setUniformMat3f("u_transform", stretch * translation);

	// Draw the screen
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}


// Draw a grid on the complex plane
void Renderer::drawGrid(SetAttributes set, int width, int height, vec2d center, double diff, bool renderJuliaSet)
{	
	// ImGui draw list init
	draw_list = ImGui::GetBackgroundDrawList();
	ImU32 colour = IM_COL32(100, 130, 200, 150);
	float axisWidth = 3.5f;
	float gridWidth = 1.0f;

	width /= (1 + renderJuliaSet);

	float sideExtra = (width * 0.5 * (set.side + 1)) * renderJuliaSet; // 0 if left, width if right
	diff -= center.x;
	
	// Draw origin crossing axis lines
	if ((0 + sideExtra) < center.x && center.x < (width + sideExtra)) {draw_list->AddLine(ImVec2(center.x, 0.0f), ImVec2(center.x, height), colour, axisWidth);} // Vertical line
	if (0 < center.y && center.y < width) {draw_list->AddLine(ImVec2(0.0f + sideExtra, center.y), ImVec2(width + sideExtra, center.y), colour, axisWidth);} // Horizontal line

	// TODO: The conditions for both lines are janky af. I wrote this very tired
	// Draw verticle grid lines
	float xPos = std::fmod(center.x - sideExtra, diff);
	for (int i = (xPos < 0.0); i < (width - xPos) / diff; i++)	{draw_list->AddLine(ImVec2(xPos + diff * i + sideExtra, 0.0f), ImVec2(xPos + diff * i + sideExtra, height), colour, gridWidth);}

	// Draw horizontal grid lines 
	float yPos = std::fmod(center.y, diff);
	for (int i = (yPos < 0.0); i < (height - yPos) / diff; i++) {draw_list->AddLine(ImVec2(0.0f + sideExtra, yPos + diff * i), ImVec2(width + sideExtra, yPos + diff * i), colour, gridWidth);}
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
	markerShader.setUniformMat3f("u_projection", projection.inverse());
	markerShader.setUniformMat3f("u_transform", translation * scale);
	markerShader.setUniformMat3f("u_translation", translation);

	markerShader.setUniformVec2f("u_markerCenter", position);

	// Draw the marker
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}

// Draw the ImGui settings for the mandelbrot set
void Renderer::drawMandelbrotSettings(SetAttributes& mandelbrotAttribs, bool& renderJuliaSet, bool& renderGrid)
{
	// ImGui::ShowDemoWindow();   
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	ImGui::SliderInt(" Max Iterations", &mandelbrotAttribs.maxIterations, 50, 1000);
	ImGui::SliderFloat(" Brightness", &mandelbrotAttribs.brightness, 50.0f, 1000.0f);
	ImGui::Text("Zoom: %.10f", mandelbrotAttribs.zoom);
	std::string posFormat = " %." + std::to_string(int(std::log10(1.0f / mandelbrotAttribs.zoom)) + 3) + "f"; // Update the format based on zoom
	ImGui::Text(("Mouse Position:" + posFormat + posFormat).c_str(), mandelbrotAttribs.complexPlanePos.x, mandelbrotAttribs.complexPlanePos.y);
	if (ImGui::Button("Reset View"))
	{
		mandelbrotAttribs.center.x = -0.5;
		mandelbrotAttribs.center.y =  0.0;
		mandelbrotAttribs.zoom = 2.0;
		mandelbrotAttribs.maxIterations = 50;
		mandelbrotAttribs.brightness = 50.0f;
	}
	ImGui::Checkbox("Show grid: ", &renderGrid);
	if (ImGui::Checkbox("Show Julia Set: ", &renderJuliaSet))
	{
		mandelbrotAttribs.maxZoom = (3.0f * (1.0f + renderJuliaSet));
		mandelbrotAttribs.enforceMaxZoom();
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		mandelbrotAttribs.enforceSetBoundaries(width, height, renderJuliaSet);
	}
	ImGui::End();
}

// Draw the ImGui settings for the marker
void Renderer::drawMarkerSettings(vec2d& markerPos)
{
	ImGui::Begin("Marker", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::InputDouble(" Marker position", &markerPos.x);

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