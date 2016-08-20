#include "FogFilter.hpp"

#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "Post.vert.hpp"
#include "PostFog.frag.hpp"

#include "../Component/Lens.hpp"

FogFilter::FogFilter(const glm::vec3& color) {
    vertexShader = Managers().resourceManager->CreateShader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(POSTFOG_FRAG, POSTFOG_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    this->color = color;
    colorLocation = shaderProgram->GetUniformLocation("color");
    
    matrixLocation = shaderProgram->GetUniformLocation("inverseProjectionMatrix");
    
    density = 0.01f;
    densityLocation = shaderProgram->GetUniformLocation("density");
}

FogFilter::~FogFilter() {
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
}

void FogFilter::SetColor(const glm::vec3& color) {
    this->color = color;
}

void FogFilter::SetDensity(float density) {
    this->density = density;
}

void FogFilter::SetCamera(const Component::Lens* lens) {
    this->lens = lens;
}

void FogFilter::SetScreenSize(const glm::vec2 &screenSize) {
    this->screenSize = screenSize;
}

ShaderProgram* FogFilter::GetShaderProgram() const {
    return shaderProgram;
}

void FogFilter::SetUniforms() {
    glUniform3fv(colorLocation, 1, &color[0]);
    glUniformMatrix4fv(matrixLocation, 1, GL_FALSE, &glm::inverse(lens->GetProjection(screenSize))[0][0]);
    glUniform1f(densityLocation, density);
}
