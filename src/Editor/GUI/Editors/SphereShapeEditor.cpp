#include "SphereShapeEditor.hpp"

#include <Engine/Component/Shape.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Physics/Shape.hpp>
#include <imgui.h>

namespace GUI {
    void SphereShapeEditor::Show(Component::Shape* comp) {
        ImGui::Indent();

        if (ImGui::DragFloat("Radius", &radius, 0.1f, 0.0f)) {
            if (radius < 0.0f)
                radius = 0.0f;

            Apply(comp);
        }

        ImGui::Unindent();
    }

    void SphereShapeEditor::Apply(Component::Shape* comp) {
        Managers().physicsManager->SetShape(comp, std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Sphere(radius))));
    }

    bool SphereShapeEditor::SetFromShape(const Physics::Shape& shape) {
        if (shape.GetKind() == Physics::Shape::Kind::Sphere) {
            auto sphereData = shape.GetSphereData();
            radius = sphereData->radius;
            return true;
        }

        return false;
    }
}
