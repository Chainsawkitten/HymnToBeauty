#include "TransformEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Transform.hpp>
#include "Vec3Editor.hpp"

using namespace GUI;

TransformEditor::TransformEditor(Widget* parent) : Widget(parent) {
    entity = nullptr;
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    
    positionEditor = new Vec3Editor(this, font);
}

TransformEditor::~TransformEditor() {
    Managers().resourceManager->FreeFont(font);
    
    delete positionEditor;
}

void TransformEditor::Update() {
    positionEditor->Update();
}

void TransformEditor::Render(const glm::vec2& screenSize) {
    if (entity != nullptr) {
        positionEditor->Render(screenSize);
    }
}

void TransformEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    positionEditor->SetPosition(position);
}

glm::vec2 TransformEditor::GetSize() const {
    return size;
}

void TransformEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    positionEditor->SetSize(size);
}

void TransformEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    Component::Transform* transform = entity->GetComponent<Component::Transform>();
    positionEditor->SetVec3(&transform->position);
}
