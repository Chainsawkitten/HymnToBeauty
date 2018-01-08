#include "CylinderShapeEditor.hpp"

#include <Engine/Component/Shape.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Physics/Shape.hpp>
#include <imgui.h>

namespace GUI {
    void CylinderShapeEditor::Show(Component::Shape* comp) {
        ImGui::Indent();

        if (ImGui::InputFloat("Radius", &radius)) {
            if (radius < 0.0f)
                radius = 0.0f;
            
            Apply(comp);
        }

        if (ImGui::InputFloat("Length", &length)) {
            if (length < 0.0f)
                length = 0.0f;
            
            Apply(comp);
        }

        ImGui::Unindent();
    }

    void CylinderShapeEditor::Apply(Component::Shape* comp) {
        Managers().physicsManager->SetShape(comp, std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Cylinder(radius, length))));
    }

    bool CylinderShapeEditor::SetFromShape(const Physics::Shape& shape) {
        if (shape.GetKind() == Physics::Shape::Kind::Cylinder) {
            auto cylinderData = shape.GetCylinderData();
            radius = cylinderData->radius;
            length = cylinderData->length;
            return true;
        }

        return false;
    }
}
