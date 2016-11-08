#include "SkinRenderProgram.hpp"

#include "../Component/Lens.hpp"
#include "../Component/Material.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Transform.hpp"
#include "../Entity/Entity.hpp"
#include "../Geometry/RiggedModel.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Texture/Texture2D.hpp"
#include "../Physics/AxisAlignedBoundingBox.hpp"
#include "../Physics/Frustum.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace Component;

SkinRenderProgram::SkinRenderProgram(ShaderProgram* shaderProgram) {
    this->shaderProgram = shaderProgram;
}

SkinRenderProgram::~SkinRenderProgram() {
    this->shaderProgram = nullptr;
}

void SkinRenderProgram::PreRender(Entity* camera, glm::vec2& screenSize) {
    shaderProgram->Use();

    viewMat = camera->GetComponent<Transform>()->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->GetComponent<Transform>()->position);
    projectionMat = camera->GetComponent<Lens>()->GetProjection(screenSize);
    viewProjectionMat = projectionMat * viewMat;

    glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewProjection"), 1, GL_FALSE, &viewProjectionMat[0][0]);
}

void SkinRenderProgram::Render(Mesh* mesh) const {
    Entity* entity = mesh->entity;
    Transform* transform = entity->GetComponent<Transform>();
    Material* material = entity->GetComponent<Material>();
    if (transform == nullptr || mesh->geometry == nullptr || material == nullptr)
        return;

    glm::mat4 modelMat = transform->GetModelMatrix();

    Physics::Frustum frustum(viewProjectionMat * modelMat);
    if (frustum.Collide(mesh->geometry->GetAxisAlignedBoundingBox())) {
        Geometry::RiggedModel* model = static_cast<Geometry::RiggedModel*>(mesh->geometry);

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
        const std::vector<glm::mat4>& bones = model->skeleton.GetFinalTransformations();
        assert(bones.size() <= 100);
        glUniformMatrix4fv(shaderProgram->GetUniformLocation("bones"), bones.size(), GL_FALSE, &bones[0][0][0]);

        glDrawElements(GL_TRIANGLES, mesh->geometry->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    }
}

void SkinRenderProgram::PostRender() const {
    glBindVertexArray(0);
}
