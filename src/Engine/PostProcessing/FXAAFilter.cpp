#include "FXAAFilter.hpp"

#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "Post.vert.hpp"
#include "PostFXAA.frag.hpp"

FXAAFilter::FXAAFilter() {
    vertexShader = Managers().resourceManager->CreateShader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(POSTFXAA_FRAG, POSTFXAA_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    screenSizeLocation = shaderProgram->GetUniformLocation("screenSize");
    brightnessLocation = shaderProgram->GetUniformLocation("brightness");
}

FXAAFilter::~FXAAFilter() {
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
}

ShaderProgram* FXAAFilter::GetShaderProgram() const {
    return shaderProgram;
}

void FXAAFilter::SetUniforms() {
    glUniform2fv(screenSizeLocation, 1, &screenSize[0]);
    glUniform1f(brightnessLocation, brightness);
}

void FXAAFilter::SetScreenSize(const glm::vec2& screenSize) {
    this->screenSize = screenSize;
}

void FXAAFilter::SetBrightness(float brightness) {
    this->brightness = brightness;
}
