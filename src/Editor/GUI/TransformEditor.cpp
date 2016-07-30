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

TransformEditor::TransformEditor(Widget* parent) : Widget(parent) {
    entity = nullptr;
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    positionLabel = new Label(this, font, "Position");
    positionEditor = new Vec3Editor(this, font);
    
    scaleLabel = new Label(this, font, "Scale");
    scaleEditor = new Vec3Editor(this, font);
    
    rotationLabel = new Label(this, font, "Rotation");
    rotationEditor = new Vec3Editor(this, font);
}

TransformEditor::~TransformEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete positionLabel;
    delete positionEditor;
    
    delete scaleLabel;
    delete scaleEditor;
    
    delete rotationLabel;
    delete rotationEditor;
}

void TransformEditor::Update() {
    positionEditor->Update();
    scaleEditor->Update();
    rotationEditor->Update();
}

void TransformEditor::Render(const glm::vec2& screenSize) {
    if (entity != nullptr) {
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
    
    positionLabel->SetPosition(pos);
    pos.y += 20.f;
    positionEditor->SetPosition(pos);
    pos.y += positionEditor->GetSize().y;
    
    scaleLabel->SetPosition(pos);
    pos.y += 20.f;
    scaleEditor->SetPosition(pos);
    pos.y += scaleEditor->GetSize().y;
    
    rotationLabel->SetPosition(pos);
    pos.y += 20.f;
    rotationEditor->SetPosition(pos);
    pos.y += rotationEditor->GetSize().y;
}

glm::vec2 TransformEditor::GetSize() const {
    return size;
}

void TransformEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    positionEditor->SetSize(size);
    scaleEditor->SetSize(size);
    rotationEditor->SetSize(size);
}

void TransformEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    Component::Transform* transform = entity->GetComponent<Component::Transform>();
    positionEditor->SetVec3(&transform->position);
    scaleEditor->SetVec3(&transform->scale);
    rotationEditor->SetVec3(&transform->rotation);
}
