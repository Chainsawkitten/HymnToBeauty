#include "OpenGLGraphicsPipeline.hpp"

#include <assert.h>
#include <Utility/Log.hpp>
#include "OpenGLShaderProgram.hpp"

namespace Video {

OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(const ShaderProgram* shaderProgram, const Configuration& configuration) {
    assert(shaderProgram != nullptr);

    this->shaderProgram = static_cast<const OpenGLShaderProgram*>(shaderProgram);
    this->configuration = configuration;
}

OpenGLGraphicsPipeline::~OpenGLGraphicsPipeline() {

}

const OpenGLShaderProgram* OpenGLGraphicsPipeline::GetShaderProgram() const {
    return shaderProgram;
}

const GraphicsPipeline::Configuration& OpenGLGraphicsPipeline::GetConfiguration() const {
    return configuration;
}

}
