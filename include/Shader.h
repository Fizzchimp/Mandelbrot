#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include "GL/glew.h"
#include "Errors.h"

class Shader
{
private:
	unsigned int shaderID;
	std::string vertexPath;
	std::string fragmentPath;

	mutable std::unordered_map<std::string, int> uniformCache;


public:
	Shader(const char* vertexPath, const char* fragmentPath);

	~Shader();

	std::string readFile(const char* filePath);
	unsigned int compileShader(unsigned int type, const std::string& source) const;

	int getUniform(const char* name) const;

	void setUniform1i(const char* name, int value) const;
	void setUniform1f(const char* name, float value) const;
	void setUniformVec2f(const char* name, const float* vector) const;
	void setUniformMat3f(const char* name, const float* matrix) const;

	void bind() const;
	void unbind() const;
};