#pragma once

#include <iostream>
#include <GL/glew.h>

#define GLCALL(func) clearErrors();\
	func;\
	if (!logCall(#func, __FILE__, __LINE__)) {__debugbreak();}


void clearErrors();

bool logCall(const char* function, const char* file, int line);