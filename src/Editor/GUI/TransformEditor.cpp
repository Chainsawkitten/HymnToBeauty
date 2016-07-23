#include "TransformEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Transform.hpp>

using namespace GUI;

TransformEditor::TransformEditor(Widget* parent) : Widget(parent) {
    entity = nullptr;
    rectangle = Managers().resourceManager->CreateRectangle();
}

TransformEditor::~TransformEditor() {
    Managers().resourceManager->FreeRectangle();
}

void TransformEditor::Update() {
    
}

void TransformEditor::Render(const glm::vec2& screenSize) {
    if (entity != nullptr) {
        Component::Transform* transform = entity->GetComponent<Component::Transform>();
        glm::vec3 color = transform != nullptr ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);
        rectangle->Render(GetPosition(), glm::vec2(20.f, 20.f), color, screenSize);
    }
}

glm::vec2 TransformEditor::GetSize() const {
    return size;
}

void TransformEditor::SetSize(const glm::vec2& size) {
    this->size = size;
}

void TransformEditor::SetEntity(Entity* entity) {
    this->entity = entity;
}
