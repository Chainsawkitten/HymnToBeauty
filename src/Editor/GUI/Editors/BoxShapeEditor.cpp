#include "BoxShapeEditor.hpp"

#include <Engine/Component/Shape.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Physics/Shape.hpp>
#include <imgui.h>

namespace GUI {
    void BoxShapeEditor::Show(Component::Shape* comp) {
        ImGui::Indent();

        if (ImGui::InputFloat("Width", &width)) {
            if (width < 0.0f)
                width = 0.0f;

            Apply(comp);
        }

        if (ImGui::InputFloat("Height", &height)) {
            if (height < 0.0f)
                height = 0.0f;

            Apply(comp);
        }

        if (ImGui::InputFloat("Depth", &depth)) {
            if (depth < 0.0f)
                depth = 0.0f;

            Apply(comp);
        }

        ImGui::Unindent();
    }

    void BoxShapeEditor::Apply(Component::Shape* comp) {
        Managers().physicsManager->SetShape(comp, std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Box(width, height, depth))));
    }

    bool BoxShapeEditor::SetFromShape(const Physics::Shape& shape) {
        if (shape.GetKind() == Physics::Shape::Kind::Box) {
            auto boxData = shape.GetBoxData();
            width = boxData->width;
            height = boxData->height;
            depth = boxData->depth;
            return true;
        }

        return false;
    }
}
