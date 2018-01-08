#include "MousePicking.hpp"

#include "Input.hpp"
#include "../MainWindow.hpp"
#include "../Entity/Entity.hpp"
#include <glm/gtx/transform.hpp>

MousePicking::MousePicking(Entity * cam,  const glm::mat4& projection) {
    this->camera = cam;
    this->pMatrix = projection;

    glm::mat4 viewMatrix = glm::inverse(cam->GetModelMatrix());
    this->vMatrix = viewMatrix;
}

MousePicking::~MousePicking() {
}

glm::vec3 MousePicking::GetCurrentRay() const {
    return this->currentRay;
}

void MousePicking::UpdateProjectionMatrix(const glm::mat4& projection) {
    this->pMatrix = projection;
}

void MousePicking::Update() {

    glm::mat4 viewMatrix = glm::inverse(camera->GetModelMatrix());
    this->vMatrix = viewMatrix;
    this->currentRay = CalculateRay();
}

glm::vec3 MousePicking::CalculateRay() const {
    double mouseX = Input()->GetCursorX();
    double mouseY = Input()->GetCursorY();

    glm::vec2 NDC = GetNDC(mouseX, mouseY);
    glm::vec4 clipSpaceCoordinates = glm::vec4(NDC.x, NDC.y, -1.0f, 1.0f);
    glm::vec4 eyeSpaceCoordinates = ConvertEyeCoords(clipSpaceCoordinates);
    glm::vec3 worldRay = ConvertWorldCoords(eyeSpaceCoordinates);

    return worldRay;
}

glm::vec2 MousePicking::GetNDC(double mouseX, double mouseY) {
    double x = (2.0f*mouseX / MainWindow::GetInstance()->GetSize().x - 1.0f);
    double y = 1.0f - (2.0f*mouseY / MainWindow::GetInstance()->GetSize().y);

    return glm::vec2(x, y);
}

glm::vec4 MousePicking::ConvertEyeCoords(const glm::vec4& clipSpaceCoordinates) const {
    glm::mat4 invertProjection = glm::inverse(pMatrix);
    glm::vec4 eyeCoords = invertProjection * clipSpaceCoordinates;

    return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
}

glm::vec3 MousePicking::ConvertWorldCoords(const glm::vec4& eyeCoords) const {
    glm::mat4 invertedView = glm::inverse(vMatrix);
    glm::vec4 rayInWorld = invertedView * eyeCoords;
    glm::vec3 mouseRay = glm::vec3(rayInWorld.x, rayInWorld.y, rayInWorld.z);
    glm::normalize(mouseRay);

    return mouseRay;
}
