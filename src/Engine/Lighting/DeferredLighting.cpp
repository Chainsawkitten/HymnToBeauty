#include "DeferredLighting.hpp"

#include "../Util/Log.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Geometry/Rectangle.hpp"
#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "Post.vert.hpp"
#include "Deferred.frag.hpp"

#include "../Entity/Entity.hpp"
#include "../Component/Transform.hpp"
#include "../Component/Lens.hpp"
#include <glm/gtc/matrix_transform.hpp>

DeferredLighting::DeferredLighting(const glm::vec2& size) {
    this->size = size;
    
    vertexShader = Managers().resourceManager->CreateShader(POST_VERT, POST_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(DEFERRED_FRAG, DEFERRED_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    rectangle = Managers().resourceManager->CreateRectangle();
    
    // Create the FBO
    glGenFramebuffers(1, &frameBufferObject);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
    
    // Generate textures
    glGenTextures(NUM_TEXTURES, textures);
    glGenTextures(1, &depthHandle);
    
    unsigned int width = static_cast<unsigned int>(size.x);
    unsigned int height = static_cast<unsigned int>(size.y);
    AttachTexture(textures[DIFFUSE], width, height, GL_COLOR_ATTACHMENT0 + DIFFUSE, GL_RGB16F);
    AttachTexture(textures[NORMAL], width, height, GL_COLOR_ATTACHMENT0 + NORMAL, GL_RGB16F);
    AttachTexture(textures[SPECULAR], width, height, GL_COLOR_ATTACHMENT0 + SPECULAR, GL_RGB);
    AttachTexture(textures[GLOW], width, height, GL_COLOR_ATTACHMENT0 + GLOW, GL_RGB);
    
    // Bind depth handle
    glBindTexture(GL_TEXTURE_2D, depthHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthHandle, 0);
    
    // Create and intialize draw buffers (output from geometry pass)
    GLenum drawBuffers[NUM_TEXTURES];
    for (unsigned int i = 0; i < NUM_TEXTURES; i++)
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(NUM_TEXTURES, drawBuffers);
    
    // Check if framebuffer created correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log() << "Frame buffer creation failed\n";
    
    // Default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

DeferredLighting::~DeferredLighting() {
    if (frameBufferObject != 0)
        glDeleteFramebuffers(1, &frameBufferObject);
    
    if (textures[0] != 0)
        glDeleteTextures(NUM_TEXTURES, textures);
    
    if (depthHandle != 0)
        glDeleteTextures(1, &depthHandle);
    
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
    
    Managers().resourceManager->FreeRectangle();
}

void DeferredLighting::SetTarget() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBufferObject);
}

void DeferredLighting::ResetTarget() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DeferredLighting::Render(Scene& scene, Entity* camera, const glm::vec2& screenSize) {
    // Disable depth testing
    GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
    
    GLint oldDepthFunctionMode;
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFunctionMode);
    glDepthFunc(GL_ALWAYS);
    
    // Blending enabled for handling multiple light sources
    GLboolean blend = glIsEnabledi(GL_BLEND, 0);
    glEnablei(GL_BLEND, 0);
    glBlendEquationi(0, GL_FUNC_ADD);
    glBlendFunci(0, GL_ONE, GL_ONE);
    
    shaderProgram->Use();
    
    BindForReading();
    glClear(GL_COLOR_BUFFER_BIT);
    
    glBindVertexArray(rectangle->GetVertexArray());
    
    // Set uniforms.
    glUniform1i(shaderProgram->GetUniformLocation("tDiffuse"), DeferredLighting::DIFFUSE);
    glUniform1i(shaderProgram->GetUniformLocation("tNormals"), DeferredLighting::NORMAL);
    glUniform1i(shaderProgram->GetUniformLocation("tSpecular"), DeferredLighting::SPECULAR);
    glUniform1i(shaderProgram->GetUniformLocation("tGlow"), DeferredLighting::GLOW);
    glUniform1i(shaderProgram->GetUniformLocation("tDepth"), DeferredLighting::NUM_TEXTURES);
    
    // Get the camera matrices.
    glm::mat4 projectionMat(camera->GetComponent<Component::Lens>()->GetProjection(screenSize));
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("inverseProjectionMatrix"), 1, GL_FALSE, &glm::inverse(projectionMat)[0][0]);
    
    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    
    if (!depthTest)
        glDisable(GL_DEPTH_TEST);
    if (!blend)
        glDisablei(GL_BLEND, 0);
    
    glDepthFunc(oldDepthFunctionMode);
}

void DeferredLighting::AttachTexture(GLuint texture, unsigned int width, unsigned int height, GLenum attachment, GLint internalFormat) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
}

void DeferredLighting::BindForReading() {
    for (unsigned int i = 0; i < NUM_TEXTURES; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
    }
    
    glActiveTexture(GL_TEXTURE0 + NUM_TEXTURES);
    glBindTexture(GL_TEXTURE_2D, depthHandle);
}
