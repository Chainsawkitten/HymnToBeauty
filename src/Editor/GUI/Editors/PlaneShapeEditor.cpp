#include "PlaneShapeEditor.hpp"

#include <Engine/Component/Shape.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Physics/Shape.hpp>
#include <glm/glm.hpp>
#include <imgui.h>

namespace GUI {
void PlaneShapeEditor::Show(Component::Shape* comp) {
    ImGui::Indent();

    if (ImGui::InputFloat3("Normal", normal))
        Apply(comp);

    if (ImGui::InputFloat("Plane coeff.", &planeCoeff))
        Apply(comp);

    ImGui::Unindent();
}

void PlaneShapeEditor::Apply(Component::Shape* comp) {
    Managers().physicsManager->SetShape(comp, std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Plane(glm::vec3(normal[0], normal[1], normal[2]), planeCoeff))));
}

bool PlaneShapeEditor::SetFromShape(const Physics::Shape& shape) {
    if (shape.GetKind() == Physics::Shape::Kind::Plane) {
        auto planeData = shape.GetPlaneData();
        normal[0] = planeData->normal.x;
        normal[1] = planeData->normal.y;
        normal[2] = planeData->normal.z;
        planeCoeff = planeData->planeCoeff;
        return true;
    }

    return false;
}
} // namespace GUI
