#include "OpenGLComputePipeline.hpp"

#include <assert.h>
#include "OpenGLShaderProgram.hpp"

namespace Video {

OpenGLComputePipeline::OpenGLComputePipeline(const ShaderProgram* shaderProgram) {
    assert(shaderProgram != nullptr);
    
    this->shaderProgram = static_cast<const OpenGLShaderProgram*>(shaderProgram);
}

OpenGLComputePipeline::~OpenGLComputePipeline() {

}

const OpenGLShaderProgram* OpenGLComputePipeline::GetShaderProgram() const {
    return shaderProgram;
}

}
