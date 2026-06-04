#include "Renderer.h"

Renderer::Renderer()
{
	GLCALL(glGenVertexArrays(1, &vertexArrayID));
	GLCALL(glBindVertexArray(vertexArrayID));

	GLCALL(glGenBuffers(1, &vertexBufferID));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID));
	GLCALL(glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertices, GL_STATIC_DRAW));

	GLCALL(glEnableVertexAttribArray(0))
	GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr));
}

Renderer::~Renderer()
{
	GLCALL(glDeleteVertexArrays(1, &vertexArrayID));
	GLCALL(glDeleteBuffers(1, &vertexBufferID));
	GLCALL(glBindVertexArray(0));
	GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}


void Renderer::draw()
{
	// Clear the screen
	GLCALL(glClear(GL_COLOR_BUFFER_BIT));

	// Draw the screen
	//GLCALL(glBindVertexArray(vertexArrayID));
	GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
}
