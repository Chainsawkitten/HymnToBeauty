#include "GammaCorrectionFilter.hpp"

#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "Post.vert.hpp"
#include "PostGamma.frag.hpp"

GammaCorrectionFilter::GammaCorrectionFilter() {
    vertexShader = Managers().resourceManager->CreateShader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(POSTGAMMA_FRAG, POSTGAMMA_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    brightnessLocation = shaderProgram->GetUniformLocation("brightness");
}

GammaCorrectionFilter::~GammaCorrectionFilter() {
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
}

ShaderProgram* GammaCorrectionFilter::GetShaderProgram() const {
    return shaderProgram;
}

void GammaCorrectionFilter::SetUniforms() {
    glUniform1f(brightnessLocation, 1.f / brightness);
}

void GammaCorrectionFilter::SetBrightness(float brightness) {
    this->brightness = brightness;
}
