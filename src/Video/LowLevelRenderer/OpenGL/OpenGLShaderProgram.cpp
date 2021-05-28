#include "OpenGLShaderProgram.hpp"

#include "OpenGLShader.hpp"
#include <Utility/Log.hpp>
#include <assert.h>

namespace Video {

OpenGLShaderProgram::OpenGLShaderProgram(std::initializer_list<const Shader*> shaders) {
    shaderProgram = glCreateProgram();

    // Link shader program.
    for (const Shader* shader : shaders)
        glAttachShader(shaderProgram, static_cast<const OpenGLShader*>(shader)->GetShaderID());

    glLinkProgram(shaderProgram);

    // Print linking errors.
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

    for (const Shader* shader : shaders) {
        // Detach shaders so that they can later be deleted.
        glDetachShader(shaderProgram, static_cast<const OpenGLShader*>(shader)->GetShaderID());

        // Get reflection information.
        const ShaderSource::ReflectionInfo& reflectionInfo = static_cast<const OpenGLShader*>(shader)->GetReflectionInfo();
        AddPushConstants(reflectionInfo);

        if (reflectionInfo.storageBufferReadWrite)
            writesToStorageBuffer = true;
    }
}

OpenGLShaderProgram::~OpenGLShaderProgram() {
    glDeleteProgram(shaderProgram);
}

unsigned int OpenGLShaderProgram::GetID() const {
    return shaderProgram;
}

const std::vector<OpenGLShaderProgram::PushConstant>& OpenGLShaderProgram::GetPushConstants() const {
    return pushConstants;
}

bool OpenGLShaderProgram::WritesToStorageBuffer() const {
    return writesToStorageBuffer;
}

void OpenGLShaderProgram::AddPushConstants(const ShaderSource::ReflectionInfo& reflectionInfo) {
    if (reflectionInfo.pushConstantCount > 0) {
        assert(reflectionInfo.pushConstants != nullptr);

        uint32_t offset = 0;
        for (unsigned int i = 0; i < reflectionInfo.pushConstantCount; ++i) {
            PushConstant pushConstant = {};
            pushConstant.uniformLocation = glGetUniformLocation(shaderProgram, (reflectionInfo.pushConstantBlockName + "." + reflectionInfo.pushConstants[i].name).c_str());
            pushConstant.type = reflectionInfo.pushConstants[i].type;
            pushConstant.offset = offset;
            offset += PushConstantTypeToSize(pushConstant.type);

            pushConstants.push_back(pushConstant);
        }
    }
}

}
