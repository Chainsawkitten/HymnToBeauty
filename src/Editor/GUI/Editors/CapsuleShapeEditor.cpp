#include "CapsuleShapeEditor.hpp"

#include <Engine/Component/Shape.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Physics/Shape.hpp>
#include <imgui.h>

namespace GUI {
void CapsuleShapeEditor::Show(Component::Shape* comp) {
    ImGui::Indent();

    if (ImGui::InputFloat("Radius", &radius)) {
        if (radius < 0.0f)
            radius = 0.0f;

        Apply(comp);
    }

    if (ImGui::InputFloat("Height", &height)) {
        if (height < 0.0f)
            height = 0.0f;

        Apply(comp);
    }

    ImGui::Unindent();
}

void CapsuleShapeEditor::Apply(Component::Shape* comp) {
    Managers().physicsManager->SetShape(comp, std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Capsule(radius, height))));
}

bool CapsuleShapeEditor::SetFromShape(const Physics::Shape& shape) {
    if (shape.GetKind() == Physics::Shape::Kind::Capsule) {
        auto capsuleData = shape.GetCapsuleData();
        radius = capsuleData->radius;
        height = capsuleData->height;
        return true;
    }

    return false;
}
} // namespace GUI
