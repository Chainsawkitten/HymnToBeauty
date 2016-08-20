#include "GlowBlurFilter.hpp"

#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "Post.vert.hpp"
#include "PostGlowBlur.frag.hpp"

GlowBlurFilter::GlowBlurFilter() {
    vertexShader = Managers().resourceManager->CreateShader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(POSTGLOWBLUR_FRAG, POSTGLOWBLUR_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    screenSizeLocation = shaderProgram->GetUniformLocation("screenSize");
    
    horizontal = false;
    horizontalLocation = shaderProgram->GetUniformLocation("horizontal");
}

GlowBlurFilter::~GlowBlurFilter() {
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
}

ShaderProgram* GlowBlurFilter::GetShaderProgram() const {
    return shaderProgram;
}

void GlowBlurFilter::SetUniforms() {
    glUniform2fv(screenSizeLocation, 1, &screenSize[0]);
    glUniform1f(horizontalLocation, horizontal);
}

void GlowBlurFilter::SetScreenSize(const glm::vec2& screenSize) {
    this->screenSize = screenSize;
}

void GlowBlurFilter::SetHorizontal(bool horizontal) {
    this->horizontal = horizontal ? 1.f : 0.f;
}
