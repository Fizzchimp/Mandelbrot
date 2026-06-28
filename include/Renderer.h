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

#include "vec2.h"
#include "mat2.h"

class Renderer
{
private:
	// Vertex array
	unsigned int vertexArrayID;

	// Vertex Buffer
	unsigned int vertexBufferID;

	// Vertices
	float vertices[12] = {
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f,

		 1.0f,  1.0f,
		-1.0f, -1.0f,
		-1.0f,  1.0f,
	};

	GLFWwindow* window; // Pointer to the window object

	mat2 projection; // Projection matrix for the shader (Based on the window dimensions)

	Shader mandelbrotShader; // Shader used to draw the mandelbrot set

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
	void drawMandelbrotSet(int maxIterations, float brightness, float zoom, vec2 center);

	// Draw various settings
	void drawSettings(int& maxIterations, float& brightness, float& zoom, vec2& complexPlanePos, vec2& center);

	// Draw the FPS counter
	void drawFPS(int width);
};