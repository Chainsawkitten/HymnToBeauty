#include "PhysicsEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Physics.hpp>
#include "../Vec3Editor.hpp"
#include "../FloatEditor.hpp"

using namespace GUI;

PhysicsEditor::PhysicsEditor(Widget* parent) : ComponentEditor(parent, "Physics") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    SetComponent<Component::Physics>();
    
    velocityEditor = new Vec3Editor(this, font);
    AddEditor("Velocity", velocityEditor);
    
    maxVelocityEditor = new FloatEditor(this, font);
    AddEditor("Max velocity", maxVelocityEditor);
    
    angularVelocityEditor = new Vec3Editor(this, font);
    AddEditor("Angular velocity", angularVelocityEditor);
    
    maxAngularVelocityEditor = new FloatEditor(this, font);
    AddEditor("Max angular velocity", maxAngularVelocityEditor);
    
    accelerationEditor = new Vec3Editor(this, font);
    AddEditor("Acceleration", accelerationEditor);
    
    angularAccelerationEditor = new Vec3Editor(this, font);
    AddEditor("Angular acceleration", angularAccelerationEditor);
    
    velocityDragFactorEditor = new FloatEditor(this, font);
    AddEditor("Velocity drag factor", velocityDragFactorEditor);
    
    angularDragFactorEditor = new FloatEditor(this, font);
    AddEditor("Angular drag factor", angularDragFactorEditor);
    
    gravityFactorEditor = new FloatEditor(this, font);
    AddEditor("Gravity factor", gravityFactorEditor);
    
    momentOfInertiaEditor = new Vec3Editor(this, font);
    AddEditor("Moment of inertia", momentOfInertiaEditor);
    
    SetVisible(false);
}

PhysicsEditor::~PhysicsEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete velocityEditor;
    delete maxVelocityEditor;
    delete angularVelocityEditor;
    delete maxAngularVelocityEditor;
    delete accelerationEditor;
    delete angularAccelerationEditor;
    delete velocityDragFactorEditor;
    delete angularDragFactorEditor;
    delete gravityFactorEditor;
    delete momentOfInertiaEditor;
}

void PhysicsEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::Physics* physics = entity->GetComponent<Component::Physics>();
        SetVisible(physics != nullptr);
        
        if (IsVisible()) {
            velocityEditor->SetVec3(&physics->velocity);
            maxVelocityEditor->SetFloat(&physics->maxVelocity);
            angularVelocityEditor->SetVec3(&physics->angularVelocity);
            maxAngularVelocityEditor->SetFloat(&physics->maxAngularVelocity);
            accelerationEditor->SetVec3(&physics->acceleration);
            angularAccelerationEditor->SetVec3(&physics->angularAcceleration);
            velocityDragFactorEditor->SetFloat(&physics->velocityDragFactor);
            angularDragFactorEditor->SetFloat(&physics->angularDragFactor);
            gravityFactorEditor->SetFloat(&physics->gravityFactor);
            momentOfInertiaEditor->SetVec3(&physics->momentOfInertia);
        }
    }
}
