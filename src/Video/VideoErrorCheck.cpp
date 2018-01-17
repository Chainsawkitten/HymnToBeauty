#include "VideoErrorCheck.hpp"

#include <GL/glew.h>
#include <Utility/Log.hpp>

VideoErrorCheck::VideoErrorCheck(const std::string& name) {
    this->name = name;
    GLenum err(glGetError());
    if (err != GL_NO_ERROR) {
        Log() << "Previous GL errors: " << name << "\n";
        do {
            Log() << (const char*)gluErrorString(err) << "\n";
            err = glGetError();
        } while (err != GL_NO_ERROR);
    }
}

VideoErrorCheck::~VideoErrorCheck() {
    GLenum err(glGetError());
    if (err != GL_NO_ERROR) {
        Log() << "Caught GL errors: " << name << "\n";
        do {
            Log() << (const char*)gluErrorString(err) << "\n";
            err = glGetError();
        } while (err != GL_NO_ERROR);
    }
}
