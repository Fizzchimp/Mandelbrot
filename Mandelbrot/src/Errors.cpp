#include "Errors.h"

void clearErrors()
{
    while (glGetError() != GL_NO_ERROR) { std::cout << glGetError(); }
}


bool logCall(const char* function, const char* file, int line)
{
    if (GLenum error = glGetError())
    {
        const char* message;
        switch (error)
        {
        case GL_INVALID_ENUM:
            message = "Invalid GLenum";
            break;

        case GL_INVALID_VALUE:
            message = "Invalid Value";
            break;

        case GL_INVALID_OPERATION:
            message = "Invalid Operation";
            break;

        case GL_STACK_OVERFLOW:
            message = "Stack Overflow";
            break;

        case GL_STACK_UNDERFLOW:
            message = "Stack Underflow";
            break;

        case GL_OUT_OF_MEMORY:
            message = "Out Of Memory";
            break;

        case GL_INVALID_FRAMEBUFFER_OPERATION:
            message = "Invalid Framebuffer Operation";
            break;

        default:
            message = "Unknown Error";
        }

        std::cout << "[OPENGL ERROR]" << std::endl
            << "Line " << line << std::endl
            << "Error: " << message << std::endl
            << "File: " << file << std::endl << std::endl;
        return false;
    }
    return true;
}