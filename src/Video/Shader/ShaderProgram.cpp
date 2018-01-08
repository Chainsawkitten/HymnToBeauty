#include "ShaderProgram.hpp"

#include "Shader.hpp"
#include "Utility/Log.hpp"
#include <vector>

using namespace Video;

ShaderProgram::ShaderProgram(std::initializer_list<const Shader*> shaders) {
    shaderProgram = glCreateProgram();

    for (const Shader* shader : shaders)
        glAttachShader(shaderProgram, shader->GetShaderID());

    glLinkProgram(shaderProgram);

    GLint isLinked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
    if (isLinked != GL_TRUE) {
        GLint maxLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

        Log() << std::string(infoLog.begin(), infoLog.end());
    }

    for (const Shader* shader : shaders)
        glDetachShader(shaderProgram, shader->GetShaderID());
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(shaderProgram);
}

void ShaderProgram::Use() const {
    glUseProgram(shaderProgram);
}

GLuint ShaderProgram::GetUniformLocation(const char* name) const {
    return glGetUniformLocation(shaderProgram, name);
}
