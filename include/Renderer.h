#pragma once

#include <iostream>
#include <cmath>

// GLEW and GLFW must be included before this file can be included
#include "Errors.h"
#include "Shader.h"
#include "GLFW/glfw3.h"
#include "GL/glew.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "SetAttributes.h"

#include "vec2.h"
#include "mat3.h"

class Renderer
{
private:
	// Vertex array
	unsigned int vertexArrayID;

	// Vertex Buffer
	unsigned int vertexBufferID;

	// Vertices
	float vertices[18] = {
		-1.0f,  1.0f, 1.0f, // Left top
		-1.0f, -1.0f, 1.0f, // Left bottom
		 1.0f,  1.0f, 1.0f, // Right top

		-1.0f, -1.0f, 1.0f, // Left bottom
		 1.0f,  1.0f, 1.0f, // Right top
		 1.0f, -1.0f, 1.0f  // Right bottom
	};

	GLFWwindow* window; // Pointer to the window object

	mat3 projection; // Projection matrix for the shader (Based on the window dimensions)
	
	// Translation matrix for rendering either set on the left or right
	mat3 translation = mat3();

	// Stretch transformation matrix for rendering both sets side by side
	mat3 stretch = mat3(
		0.5f,  0.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  0.0f,  1.0f
	);

	Shader mandelbrotShader; // Shader used to draw the mandelbrot set

	Shader juliaShader; // Shader used to draw the julia set

	Shader markerShader; // Shader used to render the marker on the mandelbrot set

public:

	Renderer(GLFWwindow* window, int width, int height);
	~Renderer();

	// Sets the projection matrix based on the width and the height of the window
    void setProjection(int width, int height);

	// Prepare the renderer for drawing elements
	void newFrame();

	// Render the new elements on screen
	void render();
	
	// Draw the mandelbrot set
	void drawMandelbrotSet(SetAttributes attributes, bool renderJuliaSet);

	// Draw the julia set
	void drawJuliaSet(SetAttributes attributes, vec2 startingPos);

	// Draw a marker on the mandelbrot set determining how the julia set is drawn
	void drawMandelbrotMarker(vec2 position, float radius);

	// Draw the ImGui settings
	void drawMandelbrotSettings(SetAttributes& mandelbrotAttribs, bool& renderJuliaSet);

	// Draw the FPS counter
	void drawFPS(int width);
};