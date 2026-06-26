#pragma once

#include <iostream>
// #include <stdexcept>
#include <cassert>
#include <GL/glew.h>


#ifdef NDEBUG
#define GLCALL(func); func;
#else
#define GLCALL(func) clearErrors();\
	func;\
	assert(logCall(#func, __FILE__, __LINE__));
#endif

void clearErrors();

bool logCall(const char* function, const char* file, int line);