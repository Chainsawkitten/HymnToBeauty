#include "RenderTarget.hpp"
#include "Util/Log.hpp"

#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"

#include "Geometry/Rectangle.hpp"

#include "Manager/Managers.hpp"
#include "Manager/ResourceManager.hpp"
#include "Post.vert.hpp"
#include "PostCopy.frag.hpp"
#include "PostDither.frag.hpp"

RenderTarget::RenderTarget(const glm::vec2& size) {
    width = static_cast<int>(size.x);
    height = static_cast<int>(size.y);
    
    // Color buffer.
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Extra buffer (eg. bloom).
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &extraBuffer);
    glBindTexture(GL_TEXTURE_2D, extraBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Depth buffer.
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &depthBuffer);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Framebuffer objects
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, extraBuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer, 0);
    
    // Initialize draw buffers.
    GLenum drawBuffers[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, drawBuffers);
    
    // Check if framebuffer created correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log() << "Framebuffer creation failed\n";
    
    // Default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    vertexShader = Managers().resourceManager->CreateShader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(POSTCOPY_FRAG, POSTCOPY_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    ditherFragmentShader = Managers().resourceManager->CreateShader(POSTDITHER_FRAG, POSTDITHER_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    ditherShaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, ditherFragmentShader });
    
    rectangle = Managers().resourceManager->CreateRectangle();
}

RenderTarget::~RenderTarget() {
    glDeleteTextures(1, &depthBuffer);
    glDeleteTextures(1, &extraBuffer);
    glDeleteTextures(1, &colorBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
    
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
    Managers().resourceManager->FreeShader(ditherFragmentShader);
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShaderProgram(ditherShaderProgram);
    
    Managers().resourceManager->FreeRectangle();
}

void RenderTarget::SetTarget() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
}

glm::vec2 RenderTarget::GetSize() const {
    return glm::vec2(static_cast<float>(width), static_cast<float>(height));
}

GLuint RenderTarget::GetColorTexture() const {
    return colorBuffer;
}

GLuint RenderTarget::GetExtraTexture() const {
    return extraBuffer;
}

GLuint RenderTarget::GetDepthTexture() const {
    return depthBuffer;
}

void RenderTarget::Render(bool dither) {
    // Always pass depth test.
    glDepthFunc(GL_ALWAYS);
    
    ShaderProgram* shader = dither ? ditherShaderProgram : shaderProgram;
    shader->Use();
    
    glUniform1i(shader->GetUniformLocation("tDiffuse"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    
    glUniform1i(shader->GetUniformLocation("tDepth"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthBuffer);
    
    if (dither) {
        glUniform1f(shader->GetUniformLocation("time"), ditherTime);
        ditherTime = fmod(ditherTime + 1.f, 255.f);
    }
    
    glBindVertexArray(rectangle->GetVertexArray());
    
    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    
    // Reset depth testing to standard value.
    glDepthFunc(GL_LESS);
}
