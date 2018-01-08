#include "RigidBodyEditor.hpp"

#include <Engine/Component/RigidBody.hpp>
#include <Engine/Component/Shape.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <imgui.h>

namespace GUI {
    void RigidBodyEditor::Show(Component::RigidBody* comp) {
        GetData(comp);

        auto shapeComp = comp->entity->GetComponent<Component::Shape>();
        if (shapeComp) {
            ImGui::Indent();

            if (ImGui::InputFloat("Mass", &mass))
                Managers().physicsManager->SetMass(comp, mass);

            float friction = comp->GetFriction();
            if (ImGui::InputFloat("Friction", &friction))
                Managers().physicsManager->SetFriction(comp, friction);

            float rollingFriction = comp->GetRollingFriction();
            if (ImGui::InputFloat("Rolling friction", &rollingFriction))
                Managers().physicsManager->SetRollingFriction(comp, rollingFriction);

            float spinningFriction = comp->GetSpinningFriction();
            if (ImGui::InputFloat("Spinning friction", &spinningFriction))
                Managers().physicsManager->SetSpinningFriction(comp, spinningFriction);

            float cor = comp->GetRestitution();
            if (ImGui::InputFloat("CoR", &cor))
                Managers().physicsManager->SetRestitution(comp, cor);

            float linearDamping = comp->GetLinearDamping();
            if (ImGui::InputFloat("Lin. damping", &linearDamping))
                Managers().physicsManager->SetLinearDamping(comp, linearDamping);

            float angularDamping = comp->GetAngularDamping();
            if (ImGui::InputFloat("Ang. damping", &angularDamping))
                Managers().physicsManager->SetAngularDamping(comp, angularDamping);

            bool kinematic = comp->IsKinematic();
            if (ImGui::Checkbox("Kinematic", &kinematic)) {
                if (kinematic)
                    Managers().physicsManager->MakeKinematic(comp);
                else
                    Managers().physicsManager->MakeDynamic(comp);
            }

            bool ghost = comp->IsGhost();
            if (ImGui::Checkbox("Ghost", &ghost))
                Managers().physicsManager->SetGhost(comp, ghost);

            ImGui::Unindent();
        }
        else {
            ImGui::Indent();
            ImGui::TextWrapped("A rigid body is only valid with a complementary shape component. Please add one to allow editing this component.");
            ImGui::Unindent();
        }
    }

    void RigidBodyEditor::GetData(Component::RigidBody* comp) {
        mass = Managers().physicsManager->GetMass(comp);
    }
}
