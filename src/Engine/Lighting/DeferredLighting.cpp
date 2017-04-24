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
#include "../Component/Lens.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/PointLight.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Physics/AxisAlignedBoundingBox.hpp"
#include "../Physics/Frustum.hpp"
#include "../MainWindow.hpp"

DeferredLighting::DeferredLighting() {
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
    
    unsigned int width = static_cast<unsigned int>(MainWindow::GetInstance()->GetSize().x);
    unsigned int height = static_cast<unsigned int>(MainWindow::GetInstance()->GetSize().y);
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
    
    // Store light uniform locations.
    for (unsigned int lightIndex = 0; lightIndex < lightCount; ++lightIndex) {
        lightUniforms[lightIndex].position = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].position").c_str());
        lightUniforms[lightIndex].intensities = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].intensities").c_str());
        lightUniforms[lightIndex].attenuation = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].attenuation").c_str());
        lightUniforms[lightIndex].ambientCoefficient = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].ambientCoefficient").c_str());
        lightUniforms[lightIndex].coneAngle = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].coneAngle").c_str());
        lightUniforms[lightIndex].direction = shaderProgram->GetUniformLocation(("lights[" + std::to_string(lightIndex) + "].direction").c_str());
    }
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

void DeferredLighting::Render(World& world, const Entity* camera) {
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
    glUniform1i(shaderProgram->GetUniformLocation("lightCount"), 32);
    
    // Get the camera matrices.
    glm::mat4 viewMat(camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->position));
    glm::mat4 projectionMat(camera->GetComponent<Component::Lens>()->GetProjection(MainWindow::GetInstance()->GetSize()));
    glm::mat4 viewProjectionMat(projectionMat * viewMat);
    
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("inverseProjectionMatrix"), 1, GL_FALSE, &glm::inverse(projectionMat)[0][0]);
    
    float cutOff;
    Physics::AxisAlignedBoundingBox aabb(glm::vec3(1.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.5f, 0.5f));
    
    unsigned int lightIndex = 0U;
    
    // Render all directional lights.
    std::vector<Component::DirectionalLight*>& directionalLights = world.GetComponents<Component::DirectionalLight>();
    for (Component::DirectionalLight* light : directionalLights) {
        Entity* lightEntity = light->entity;
        glm::vec4 direction(glm::vec4(lightEntity->GetDirection(), 0.f));
        glUniform4fv(lightUniforms[lightIndex].position, 1, &(viewMat * -direction)[0]);
        glUniform3fv(lightUniforms[lightIndex].intensities, 1, &light->color[0]);
        glUniform1f(lightUniforms[lightIndex].attenuation, 1.f);
        glUniform1f(lightUniforms[lightIndex].ambientCoefficient, light->ambientCoefficient);
        glUniform1f(lightUniforms[lightIndex].coneAngle, 0.f);
        glUniform3fv(lightUniforms[lightIndex].direction, 1, &glm::vec3(0.f, 0.f, 0.f)[0]);
        
        if (++lightIndex >= lightCount) {
            lightIndex = 0U;
            glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
        }
    }
    
    // Render all spot lights.
    std::vector<Component::SpotLight*>& spotLights = world.GetComponents<Component::SpotLight>();
    for (Component::SpotLight* light : spotLights) {
        Entity* lightEntity = light->entity;
        glm::vec4 direction(viewMat * glm::vec4(lightEntity->GetDirection(), 0.f));
        glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
        glUniform4fv(lightUniforms[lightIndex].position, 1, &(viewMat * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0)))[0]);
        glUniform3fv(lightUniforms[lightIndex].intensities, 1, &(light->color * light->intensity)[0]);
        glUniform1f(lightUniforms[lightIndex].attenuation, light->attenuation);
        glUniform1f(lightUniforms[lightIndex].ambientCoefficient, light->ambientCoefficient);
        glUniform1f(lightUniforms[lightIndex].coneAngle, light->coneAngle);
        glUniform3fv(lightUniforms[lightIndex].direction, 1, &glm::vec3(direction)[0]);
        
        if (++lightIndex >= lightCount) {
            lightIndex = 0U;
            glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
        }
    }
    
    // At which point lights should be cut off (no longer contribute).
    cutOff = 0.0001f;
    
    // Render all point lights.
    std::vector<Component::PointLight*>& pointLights = world.GetComponents<Component::PointLight>();
    for (Component::PointLight* light : pointLights) {
        Entity* lightEntity = light->entity;
        float scale = sqrt((1.f / cutOff - 1.f) / light->attenuation);
        glm::mat4 modelMat = glm::translate(glm::mat4(), lightEntity->position) * glm::scale(glm::mat4(), glm::vec3(1.f, 1.f, 1.f) * scale);
        
        Physics::Frustum frustum(viewProjectionMat * modelMat);
        if (frustum.Collide(aabb)) {
            glm::mat4 modelMatrix(lightEntity->GetModelMatrix());
            glUniform4fv(lightUniforms[lightIndex].position, 1, &(viewMat * (glm::vec4(glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]), 1.0)))[0]);
            glUniform3fv(lightUniforms[lightIndex].intensities, 1, &(light->color * light->intensity)[0]);
            glUniform1f(lightUniforms[lightIndex].attenuation, light->attenuation);
            glUniform1f(lightUniforms[lightIndex].ambientCoefficient, light->ambientCoefficient);
            glUniform1f(lightUniforms[lightIndex].coneAngle, 180.f);
            glUniform3fv(lightUniforms[lightIndex].direction, 1, &glm::vec3(1.f, 0.f, 0.f)[0]);
            
            if (++lightIndex >= lightCount) {
                lightIndex = 0U;
                glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
            }
        }
    }
    
    if (lightIndex != 0U) {
        glUniform1i(shaderProgram->GetUniformLocation("lightCount"), lightIndex);
        glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    }
    
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
