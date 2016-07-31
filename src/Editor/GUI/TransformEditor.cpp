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

TransformEditor::TransformEditor(Widget* parent) : ComponentEditor(parent) {
    entity = nullptr;
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    transformLabel = new Label(this, font, "Transform");
    
    positionLabel = new Label(this, font, "Position");
    positionEditor = new Vec3Editor(this, font);
    
    scaleLabel = new Label(this, font, "Scale");
    scaleEditor = new Vec3Editor(this, font);
    
    rotationLabel = new Label(this, font, "Rotation");
    rotationEditor = new Vec3Editor(this, font);
    
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

void TransformEditor::Update() {
    if (IsVisible()) {
        positionEditor->Update();
        scaleEditor->Update();
        rotationEditor->Update();
    }
}

void TransformEditor::Render(const glm::vec2& screenSize) {
    if (IsVisible()) {
        transformLabel->Render(screenSize);
        
        positionLabel->Render(screenSize);
        positionEditor->Render(screenSize);
        
        scaleLabel->Render(screenSize);
        scaleEditor->Render(screenSize);
        
        rotationLabel->Render(screenSize);
        rotationEditor->Render(screenSize);
    }
}

void TransformEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    glm::vec2 pos(position);
    
    transformLabel->SetPosition(pos);
    pos += glm::vec2(10.f, 20.f);
    
    positionLabel->SetPosition(pos);
    pos.y += 20.f;
    positionEditor->SetPosition(pos + glm::vec2(10.f, 0.f));
    pos.y += positionEditor->GetSize().y + 5.f;
    
    scaleLabel->SetPosition(pos);
    pos.y += 20.f;
    scaleEditor->SetPosition(pos + glm::vec2(10.f, 0.f));
    pos.y += scaleEditor->GetSize().y + 5.f;
    
    rotationLabel->SetPosition(pos);
    pos.y += 20.f;
    rotationEditor->SetPosition(pos + glm::vec2(10.f, 0.f));
    pos.y += rotationEditor->GetSize().y + 5.f;
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
    this->entity = entity;
    
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
