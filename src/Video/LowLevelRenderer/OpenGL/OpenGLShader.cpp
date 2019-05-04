#include "OpenGLShader.hpp"

#include <vector>
#include <Utility/Log.hpp>
#include <assert.h>

namespace Video {

OpenGLShader::OpenGLShader(const ShaderSource& shaderSource, Type type) {
    assert(shaderSource.glslSource != nullptr);

    // Get OpenGL shader type.
    GLenum shaderType;
    switch (type) {
    case Type::VERTEX_SHADER:
        shaderType = GL_VERTEX_SHADER;
        break;
    case Type::FRAGMENT_SHADER:
        shaderType = GL_FRAGMENT_SHADER;
        break;
    default:
        assert(false);
    }

    // Create the shader.
    shader = glCreateShader(shaderType);
    GLint length = shaderSource.glslLength;
    glShaderSource(shader, 1, &shaderSource.glslSource, &length);
    glCompileShader(shader);

    // Check for shader compilation errors.
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

        Log(Log::ERR) << std::string(infoLog.begin(), infoLog.end());
    }
    
    reflectionInfo = shaderSource.reflectionInfo;
}

OpenGLShader::~OpenGLShader() {
    glDeleteShader(shader);
}

GLuint OpenGLShader::GetShaderID() const {
    return shader;
}

const ShaderSource::ReflectionInfo& OpenGLShader::GetReflectionInfo() const {
    return reflectionInfo;
}

}
