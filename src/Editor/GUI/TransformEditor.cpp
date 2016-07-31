#include "TransformEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Transform.hpp>
#include "Label.hpp"
#include "Vec3Editor.hpp"

using namespace GUI;

TransformEditor::TransformEditor(Widget* parent) : ComponentEditor(parent, "Transform") {
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    transformLabel = new Label(this, font, "Transform");
    
    positionLabel = new Label(this, font, "Position");
    positionEditor = new Vec3Editor(this, font);
    AddEditor("Position", positionEditor);
    
    scaleLabel = new Label(this, font, "Scale");
    scaleEditor = new Vec3Editor(this, font);
    AddEditor("Scale", scaleEditor);
    
    rotationLabel = new Label(this, font, "Rotation");
    rotationEditor = new Vec3Editor(this, font);
    AddEditor("Rotation", rotationEditor);
    
    SetVisible(false);
}

TransformEditor::~TransformEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete transformLabel;
    
    delete positionLabel;
    delete positionEditor;
    
    delete scaleLabel;
    delete scaleEditor;
    
    delete rotationLabel;
    delete rotationEditor;
}

glm::vec2 TransformEditor::GetSize() const {
    return size;
}

void TransformEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    positionEditor->SetSize(size + glm::vec2(-20.f, 0.f));
    scaleEditor->SetSize(size + glm::vec2(-20.f, 0.f));
    rotationEditor->SetSize(size + glm::vec2(-20.f, 0.f));
    
    this->size.y = 85.f + positionEditor->GetSize().y + scaleEditor->GetSize().y + rotationEditor->GetSize().y;
}

void TransformEditor::SetEntity(Entity* entity) {
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::Transform* transform = entity->GetComponent<Component::Transform>();
        SetVisible(transform != nullptr);
        
        if (IsVisible()) {
            positionEditor->SetVec3(&transform->position);
            scaleEditor->SetVec3(&transform->scale);
            rotationEditor->SetVec3(&transform->rotation);
        }
    }
}
