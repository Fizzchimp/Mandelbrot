#pragma once

// GLEW and GLFW must be included before this file can be included
#include "Errors.h"
#include "Shader.h"
#include "GL/glew.h"

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

public:
	Renderer();
	~Renderer();

	void draw();
};