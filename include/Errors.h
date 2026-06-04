#pragma once

#include <iostream>
#include <GL/glew.h>

#define GLCALL(func) clearErrors();\
	func;\
	if (!logCall(#func, __FILE__, __LINE__)) {}


void clearErrors();

bool logCall(const char* function, const char* file, int line);