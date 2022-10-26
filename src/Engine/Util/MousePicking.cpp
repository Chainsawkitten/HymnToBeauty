#include "MousePicking.hpp"

#include "Input.hpp"
#include "../Entity/Entity.hpp"
#include <glm/gtx/transform.hpp>
#include <Utility/Window.hpp>

namespace MousePicking {

static glm::vec2 GetNDC(double mouseX, double mouseY, const Utility::Window* window) {
    double x = (2.0f * mouseX / window->GetSize().x - 1.0f);
    double y = 1.0f - (2.0f * mouseY / window->GetSize().y);

    return glm::vec2(x, y);
}

static glm::vec4 ConvertEyeCoords(const glm::vec4& clipSpaceCoordinates, const glm::mat4 projection) {
    glm::mat4 invertProjection = glm::inverse(projection);
    glm::vec4 eyeCoords = invertProjection * clipSpaceCoordinates;

    return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

static glm::vec3 ConvertWorldCoords(const glm::vec4& eyeCoords, const glm::mat4& viewMatrix) {
    glm::mat4 invertedView = glm::inverse(viewMatrix);
    glm::vec4 rayInWorld = invertedView * eyeCoords;
    glm::vec3 mouseRay = glm::vec3(rayInWorld.x, rayInWorld.y, rayInWorld.z);
    glm::normalize(mouseRay);

    return mouseRay;
}

glm::vec3 GetRayDirection(const Entity* camera, const glm::mat4& projection, const Utility::Window* window) {
    const glm::mat4 viewMatrix = glm::inverse(camera->GetModelMatrix());
    double mouseX = Input()->GetCursorX();
    double mouseY = Input()->GetCursorY();

    glm::vec2 NDC = GetNDC(mouseX, mouseY, window);
    glm::vec4 clipSpaceCoordinates = glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);
    glm::vec4 eyeSpaceCoordinates = ConvertEyeCoords(clipSpaceCoordinates, projection);
    glm::vec3 worldRay = ConvertWorldCoords(eyeSpaceCoordinates, viewMatrix);

    return worldRay;
}

} // namespace MousePicking
