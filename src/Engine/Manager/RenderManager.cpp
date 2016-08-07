#include "RenderManager.hpp"

#include <GL/glew.h>
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "Default3D.vert.hpp"
#include "Default3D.frag.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Transform.hpp"
#include "../Component/Mesh.hpp"
#include "../Geometry/Geometry3D.hpp"
#include "../Lighting/DeferredLighting.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace Component;

RenderManager::RenderManager(const glm::vec2& screenSize) {
    vertexShader = Managers().resourceManager->CreateShader(DEFAULT3D_VERT, DEFAULT3D_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(DEFAULT3D_FRAG, DEFAULT3D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
    
    deferredLighting = new DeferredLighting(screenSize);
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    
    delete deferredLighting;
}

void RenderManager::Render(Scene& scene, const glm::vec2& screenSize) {
    deferredLighting->SetTarget();
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
    
    // Find camera entity.
    Entity* camera = nullptr;
    std::vector<Lens*> lenses = scene.GetComponents<Lens>();
    for (Lens* lens : lenses) {
        if (lens->entity->GetComponent<Transform>() != nullptr)
            camera = lens->entity;
    };
    
    // Render from camera.
    if (camera != nullptr) {
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
            if (transform != nullptr && mesh->geometry != nullptr) {
                glm::mat4 modelMat = transform->GetModelMatrix();
                
                glBindVertexArray(mesh->geometry->GetVertexArray());
                
                // Render model.
                glUniformMatrix4fv(shaderProgram->GetUniformLocation("model"), 1, GL_FALSE, &modelMat[0][0]);
                glm::mat4 normalMat = glm::transpose(glm::inverse(viewMat * modelMat));
                glUniformMatrix3fv(shaderProgram->GetUniformLocation("normalMatrix"), 1, GL_FALSE, &glm::mat3(normalMat)[0][0]);
                
                glDrawElements(GL_TRIANGLES, mesh->geometry->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
            }
        }
        
        glBindVertexArray(0);
    }
    
    // Light the scene.
    deferredLighting->ResetTarget();
    deferredLighting->Render(scene, camera, screenSize);
}
