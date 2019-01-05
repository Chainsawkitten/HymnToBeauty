#include "FXAAFilter.hpp"

#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "Post.vert.hpp"
#include "PostFXAA.frag.hpp"

using namespace Video;

FXAAFilter::FXAAFilter() {
    Shader* vertexShader = new Shader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(POSTFXAA_FRAG, POSTFXAA_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new ShaderProgram({vertexShader, fragmentShader});
    delete vertexShader;
    delete fragmentShader;

    screenSizeLocation = shaderProgram->GetUniformLocation("screenSize");
    diffuseLocation = shaderProgram->GetUniformLocation("tDiffuse");
}

FXAAFilter::~FXAAFilter() {
    delete shaderProgram;
}

ShaderProgram* FXAAFilter::GetShaderProgram() const {
    return shaderProgram;
}

GLuint FXAAFilter::GetDiffuseLocation() const {
    return diffuseLocation;
}

void FXAAFilter::SetUniforms() {
    glUniform2fv(screenSizeLocation, 1, &screenSize[0]);
}

void FXAAFilter::SetScreenSize(const glm::vec2& screenSize) {
    this->screenSize = screenSize;
}
