#include "TransformEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Transform.hpp>
#include "FloatEditor.hpp"

using namespace GUI;

TransformEditor::TransformEditor(Widget* parent) : Widget(parent) {
    entity = nullptr;
    rectangle = Managers().resourceManager->CreateRectangle();
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    xEditor = new FloatEditor(this, font);
    xEditor->SetPosition(GetPosition());
}

TransformEditor::~TransformEditor() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
}

void TransformEditor::Update() {
    xEditor->Update();
}

void TransformEditor::Render(const glm::vec2& screenSize) {
    if (entity != nullptr) {
        Component::Transform* transform = entity->GetComponent<Component::Transform>();
        glm::vec3 color = transform != nullptr ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);
        rectangle->Render(GetPosition(), glm::vec2(20.f, 20.f), color, screenSize);
        
        xEditor->Render(screenSize);
    }
}

void TransformEditor::SetPosition(const glm::vec2& position) {
    Widget::SetPosition(position);
    
    xEditor->SetPosition(position);
}

glm::vec2 TransformEditor::GetSize() const {
    return size;
}

void TransformEditor::SetSize(const glm::vec2& size) {
    this->size = size;
    
    xEditor->SetSize(size);
}

void TransformEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    Component::Transform* transform = entity->GetComponent<Component::Transform>();
    xEditor->SetFloat(&transform->position.x);
}
