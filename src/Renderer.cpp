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
void Renderer::drawGrid(SetAttributes set, int width, int height, bool renderJuliaSet)
{	
	double complexToWindowScale = std::min(width, height) / set.zoom;
	
	// Calculates the distance two lines on the graph will be (window distance)
	double complexDiff = std::pow(2, std::ilogb(set.zoom) - renderJuliaSet - 1); // Distance between drawn lines (complex distance)
	double diff = complexDiff / set.zoom * std::min(width, height) / 2; // Distance between drawn lines (window distance)
	
	vec2d originPos = (vec2d(-set.center.x, set.center.y) * complexToWindowScale + vec2d(width * renderJuliaSet / 2 * set.side + width, height)) / 2; // position of the origin (window coordinates) (problems due to floating point errors) // OLD
	
	width /= (1 + renderJuliaSet);
	float sideExtra = (width * 0.5 * (set.side + 1)) * renderJuliaSet; // 0 if left, width if right

	vec2d centralLinesOffset = vec2d(std::fmod(set.center.x, complexDiff), std::fmod(set.center.y, complexDiff)); // Offset from the screen center where the lines are drawn (complex coords)
	vec2d centralLinesPos = vec2d(std::fmod((-centralLinesOffset.x * complexToWindowScale + width) / 2, diff), std::fmod((centralLinesOffset.y * complexToWindowScale + height) / 2, diff)); // Position of the first lines drawn (window coords)
	
	vec2d linesBeforeCentral = vec2d(int((-centralLinesOffset.x * complexToWindowScale + width) / 2 / diff), -int((centralLinesOffset.y * complexToWindowScale + height) / 2 / diff));
	vec2d firstNumber = set.center - centralLinesOffset - linesBeforeCentral * complexDiff;
	
	
	// ImGui draw list init
	drawList = ImGui::GetBackgroundDrawList();
	ImU32 colour = IM_COL32(125, 125, 150, 150);
	ImU32 textCol = IM_COL32(225, 225, 250, 255);
	float axisWidth = 3.5f;
	float gridWidth = 1.0f;
	
	
	
	// Draw origin crossing axis lines
	if ((0 + sideExtra) < originPos.x && originPos.x < (width + sideExtra)) {drawList->AddLine(ImVec2(originPos.x, 0.0f), ImVec2(originPos.x, height), colour, axisWidth);} // Vertical line
	if (0 < originPos.y && originPos.y < height) {drawList->AddLine(ImVec2(0.0f + sideExtra, originPos.y), ImVec2(width + sideExtra, originPos.y), colour, axisWidth);} // Horizontal line

	// Draw origin axis number
	vec2 textOffset = vec2(7.0f, 5.0f);
	if (0 + sideExtra < originPos.x + textOffset.x && originPos.x + textOffset.x < width + sideExtra && 0 < originPos.y + textOffset.y && originPos.y + textOffset.y < height)
	{drawList->AddText(NULL, 13, ImVec2(originPos.x + textOffset.x, originPos.y + textOffset.y), IM_COL32(225, 225, 250, 255), "0");} // Origin
	
	
	//// Draw grid lines

	// String to display the text
	std::string numString;

	// Position of the numbers on the axis
	float yPos;
	// Vertical lines (x changes)
	for (int i = 0; i < (width - centralLinesPos.x) / diff; i++)
	{
		if (firstNumber.x + complexDiff * i != 0)
		{
			// Draw line
			drawList->AddLine(ImVec2(centralLinesPos.x + diff * i + sideExtra, 0.0f), ImVec2(centralLinesPos.x + diff * i + sideExtra, height), colour, gridWidth);

			// Draw number
			numString = std::to_string(firstNumber.x + complexDiff * i);
			numString.erase(numString.find_last_not_of("0") + 1, std::string::npos);
			numString.erase(numString.find_last_not_of(".") + 1, std::string::npos);

			if (originPos.y < 0.0) {yPos = textOffset.x;}
			else if (originPos.y > height - 2 * textOffset.y - 15) {yPos = height - textOffset.y - 15;}
			else {yPos = originPos.y + textOffset.x;}

			drawList->AddText(ImVec2(centralLinesPos.x + diff * i + textOffset.x + sideExtra, yPos), textCol, numString.c_str());
		}
	}

	// Position of the numbers on the axis
	float xPos;
	// Horizontal lines (y changes)
	for (int i = 0; i < (height - centralLinesPos.y) / diff; i++)
	{
		if (firstNumber.y - complexDiff * i != 0)
		{
			drawList->AddLine(ImVec2(0.0f + sideExtra, centralLinesPos.y + diff * i), ImVec2(width + sideExtra, centralLinesPos.y + diff * i), colour, gridWidth);
			
			// Draw number
			numString = std::to_string(firstNumber.y - complexDiff * i);
			numString.erase(numString.find_last_not_of("0") + 1, std::string::npos);
			numString.erase(numString.find_last_not_of(".") + 1, std::string::npos);

			if (originPos.x - sideExtra < 0.0) {xPos = textOffset.x + sideExtra;}
			else if (originPos.x - sideExtra> (width - 7 * numString.length() - 2 * textOffset.x)) {xPos = width - 7 * numString.length() - textOffset.x + sideExtra;}
			else {xPos = originPos.x + textOffset.x;}

			drawList->AddText(ImVec2(xPos, centralLinesPos.y + diff * i + textOffset.y), textCol, numString.c_str());
		}
	}
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
void Renderer::drawMandelbrotSettings(SetAttributes& mandelbrotAttribs, bool& renderJuliaSet, bool& renderGrid, int width)
{
	// ImGui::ShowDemoWindow();   
	ImGui::SetNextWindowPos(ImVec2(width * (1 + mandelbrotAttribs.side) / 4 * renderJuliaSet + 60, 60));
	ImGui::Begin("Mandelbrot Settings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
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
	ImGui::Text("Hold left click to move around. Use the scrollwheel to zoom in / out.");
	ImGui::End();
}

// Draw the ImGui settings for the julia set
void Renderer::drawJuliaSettings(SetAttributes &juliaAttribs, int width)
{
	ImGui::SetNextWindowPos(ImVec2(width * (1 + juliaAttribs.side) / 4 + 60, 60));
	ImGui::Begin("Julia Settings", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	ImGui::SliderInt(" Max Iterations", &juliaAttribs.maxIterations, 50, 1000);
	ImGui::SliderFloat(" Brightness", &juliaAttribs.brightness, 50.0f, 1000.0f);
	ImGui::Text("Zoom: %.10f", juliaAttribs.zoom);
	std::string posFormat = " %." + std::to_string(int(std::log10(1.0f / juliaAttribs.zoom)) + 3) + "f"; // Update the format based on zoom
	ImGui::Text(("Mouse Position:" + posFormat + posFormat).c_str(), juliaAttribs.complexPlanePos.x, juliaAttribs.complexPlanePos.y);
	if (ImGui::Button("Reset View"))
	{
		juliaAttribs.center.x = 0.0;
		juliaAttribs.center.y = 0.0;
		juliaAttribs.zoom = 2.0;
		juliaAttribs.maxIterations = 50;
		juliaAttribs.brightness = 50.0f;
	}
	ImGui::End();
}

// Draw the ImGui settings for the marker
void Renderer::drawMarkerSettings(vec2d& markerPos)
{
	ImGui::Begin("Marker", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::InputDouble(" ", &markerPos.x);
	ImGui::SameLine();
	ImGui::InputDouble(" Marker Position", &markerPos.y);

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