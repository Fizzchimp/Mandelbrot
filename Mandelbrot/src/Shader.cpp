#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
	: vertexPath(vertexPath), fragmentPath(fragmentPath)
{
	std::string vertexSource = readFile(vertexPath);
	std::string fragmentSource = readFile(fragmentPath);

	GLCALL(shaderID = glCreateProgram());
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

	GLCALL(glAttachShader(shaderID, vs));
	GLCALL(glAttachShader(shaderID, fs));
	GLCALL(glLinkProgram(shaderID));
	GLCALL(glValidateProgram(shaderID));

	GLCALL(glDeleteShader(vs));
	GLCALL(glDeleteShader(fs));
}

Shader::~Shader()
{
	GLCALL(glDeleteProgram(shaderID));
}

std::string Shader::readFile(const char* filePath)
{
	std::string contents;
	std::ifstream file_stream(filePath);

	std::getline(file_stream, contents, '\0');
	return contents;
}

unsigned int Shader::compileShader(unsigned int type, const std::string& source) const
{
	GLCALL(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCALL(glShaderSource(id, 1, &src, nullptr));
	GLCALL(glCompileShader(id));

	// Check shader compiled succesfully
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;
		__debugbreak();

		glDeleteShader(id);
	}
	return id;
}

int Shader::getUniform(const char* name) const
{
	if (uniformCache.find(name) != uniformCache.end()) { return uniformCache[name]; }

	GLCALL(int location = glGetUniformLocation(shaderID, name));
	if (location == -1)
	{
		std::cout << "Uniform not found: " << name << std::endl;
		__debugbreak();
	}
	uniformCache[name] = location;
	return location;
}

void Shader::setUniform1i(const char* name, int value) const
{
	GLCALL(glUniform1i(getUniform(name), value));
}

void Shader::setUniform1f(const char* name, float value) const
{
	GLCALL(glUniform1f(getUniform(name), value));
}

//void Shader::setUniform4f(const char* name, float f0, float f1, float f2, float f3) const
//{
//	GLCALL(glUniform4f(getUniform(name), f0, f1, f2, f3));
//}

//void Shader::setUniformMat4f(const char* name, const float* matrix) const
//{
//	GLCALL(glUniformMatrix4fv(getUniform(name), 1, GL_TRUE, matrix));
//}

void Shader::setUniformVec2f(const char* name, const float* vector) const
{
	GLCALL(glUniform2fv(getUniform(name), 1, vector));
}

void Shader::setUniformMat2f(const char* name, const float* matrix) const
{
	GLCALL(glUniformMatrix2fv(getUniform(name), 1, GL_TRUE, matrix));
}

void Shader::bind() const
{
	GLCALL(glUseProgram(shaderID));
}

void Shader::unbind() const
{
	GLCALL(glUseProgram(0));
}
