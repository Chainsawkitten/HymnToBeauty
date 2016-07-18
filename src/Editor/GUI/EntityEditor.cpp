#include "EntityEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>

using namespace GUI;

EntityEditor::EntityEditor(Widget* parent) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
}

EntityEditor::~EntityEditor() {
    Managers().resourceManager->FreeRectangle();
}

void EntityEditor::Update() {
    
}

void EntityEditor::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
}

glm::vec2 EntityEditor::GetSize() const {
    return size;
}

void EntityEditor::SetSize(const glm::vec2& size) {
    this->size = size;
}
