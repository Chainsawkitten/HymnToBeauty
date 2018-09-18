#include "VideoErrorCheck.hpp"

#include <Utility/Log.hpp>

VideoErrorCheck::VideoErrorCheck(const std::string& name) {
    this->name = name;
    CheckError("Previous GL errors: ");
}

VideoErrorCheck::~VideoErrorCheck() {
    CheckError("Caught GL errors: ");
}

void VideoErrorCheck::CheckError(const char* message) {
    GLenum err(glGetError());
    if (err != GL_NO_ERROR) {
        Log() << message << name << "\n";
        do {
            Log() << GetErrorString(err) << "\n";
            err = glGetError();
        } while (err != GL_NO_ERROR);
    }
}

const char* VideoErrorCheck::GetErrorString(GLenum error) {
    switch (error) {
    case GL_NO_ERROR:
        return "No error";
    case GL_INVALID_ENUM:
        return "Invalid enum";
    case GL_INVALID_VALUE:
        return "Invalid value";
    case GL_INVALID_OPERATION:
        return "Invalid operation";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "Invalid framebuffer operation";
    case GL_OUT_OF_MEMORY:
        return "Out of memory";
    case GL_STACK_UNDERFLOW:
        return "Stack underflow";
    case GL_STACK_OVERFLOW:
        return "Stack overflow";
    default:
        return "Unknown error";
    }
}
