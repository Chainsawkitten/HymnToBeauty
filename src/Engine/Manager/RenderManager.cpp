#include "RenderManager.hpp"

#include <GL/glew.h>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "ParticleManager.hpp"
#include "Default3D.vert.hpp"
#include "Default3D.frag.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Transform.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Geometry/Geometry3D.hpp"
#include "../Texture/Texture2D.hpp"
#include "../Lighting/DeferredLighting.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Physics/Frustum.hpp"
#include "../MainWindow.hpp"
#include "../RenderTarget.hpp"
#include "../PostProcessing/PostProcessing.hpp"
#include "../PostProcessing/FXAAFilter.hpp"
#include "../PostProcessing/GammaCorrectionFilter.hpp"
#include "../PostProcessing/GlowFilter.hpp"
#include "../PostProcessing/GlowBlurFilter.hpp"

using namespace Component;

RenderManager::RenderManager() {
    vertexShader = Managers().resourceManager->CreateShader(DEFAULT3D_VERT, DEFAULT3D_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(DEFAULT3D_FRAG, DEFAULT3D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    deferredLighting = new DeferredLighting();
    
    postProcessing = new PostProcessing();
    fxaaFilter = new FXAAFilter();
    gammaCorrectionFilter = new GammaCorrectionFilter();
    glowFilter = new GlowFilter();
    glowBlurFilter = new GlowBlurFilter();
    
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    
    delete deferredLighting;
    
    delete postProcessing;
    delete fxaaFilter;
    delete gammaCorrectionFilter;
    delete glowFilter;
    delete glowBlurFilter;
}

void RenderManager::Render(Scene& scene) {
    // Find camera entity.
    Entity* camera = nullptr;
    std::vector<Lens*> lenses = scene.GetComponents<Lens>();
    for (Lens* lens : lenses) {
        if (lens->entity->GetComponent<Transform>() != nullptr)
            camera = lens->entity;
    };
    
    // Render from camera.
    if (camera != nullptr) {
        deferredLighting->SetTarget();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec2 screenSize(MainWindow::GetInstance()->GetSize());
        glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
        
        shaderProgram->Use();
        
        glm::mat4 viewMat = camera->GetComponent<Component::Transform>()->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetComponent<Transform>()->position);
        glm::mat4 projectionMat = camera->GetComponent<Component::Lens>()->GetProjection(screenSize);
        glm::mat4 viewProjectionMat = projectionMat * viewMat;
        
        glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewProjection"), 1, GL_FALSE, &viewProjectionMat[0][0]);
        
        // Finds models in scene.
        std::vector<Mesh*> meshes = scene.GetComponents<Mesh>();
        for (Mesh* mesh : meshes) {
            Entity* model = mesh->entity;
            Transform* transform = model->GetComponent<Component::Transform>();
            Component::Material* material = model->GetComponent<Component::Material>();
            if (transform != nullptr && mesh->geometry != nullptr && material != nullptr) {
                glm::mat4 modelMat = transform->GetModelMatrix();
                
                Physics::Frustum frustum(viewProjectionMat * modelMat);
                if (frustum.Collide(mesh->geometry->GetAxisAlignedBoundingBox())) {
                    glBindVertexArray(mesh->geometry->GetVertexArray());
                    
                    // Set texture locations
                    glUniform1i(shaderProgram->GetUniformLocation("baseImage"), 0);
                    glUniform1i(shaderProgram->GetUniformLocation("normalMap"), 1);
                    glUniform1i(shaderProgram->GetUniformLocation("specularMap"), 2);
                    glUniform1i(shaderProgram->GetUniformLocation("glowMap"), 3);
                    
                    // Textures
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, material->diffuse->GetTextureID());
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, material->normal->GetTextureID());
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, material->specular->GetTextureID());
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_2D, material->glow->GetTextureID());
                    
                    // Render model.
                    glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &modelMat[0][0]);
                    glm::mat4 normalMat = glm::transpose(glm::inverse(viewMat * modelMat));
                    glUniformMatrix3fv(shaderProgram->GetUniformLocation("normalMatrix"), 1, GL_FALSE, &glm::mat3(normalMat)[0][0]);
                    
                    glDrawElements(GL_TRIANGLES, mesh->geometry->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
                }
            }
        }
        
        glBindVertexArray(0);
        
        // Light the scene.
        postProcessing->GetRenderTarget()->SetTarget();
        deferredLighting->Render(scene, camera);
        
        // Anti-aliasing.
        fxaaFilter->SetScreenSize(screenSize);
        postProcessing->ApplyFilter(fxaaFilter);
        
        // Render particles.
        Managers().particleManager->UpdateBuffer(scene);
        Managers().particleManager->Render(scene, camera);
        
        // Glow.
        glowBlurFilter->SetScreenSize(screenSize);
        int blurAmount = 1;
        for (int i = 0; i < blurAmount; ++i) {
            glowBlurFilter->SetHorizontal(true);
            postProcessing->ApplyFilter(glowBlurFilter);
            glowBlurFilter->SetHorizontal(false);
            postProcessing->ApplyFilter(glowBlurFilter);
        }
        postProcessing->ApplyFilter(glowFilter);
        
        // Gamma correction.
        postProcessing->ApplyFilter(gammaCorrectionFilter);
        
        // Render to back buffer.
        postProcessing->Render(true);
    }
}
