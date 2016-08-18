#include "VerticalLayout.hpp"

#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>

using namespace GUI;

VerticalLayout::VerticalLayout(Widget* parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
}

VerticalLayout::~VerticalLayout() {
    Managers().resourceManager->FreeRectangle();
}

void VerticalLayout::Render(const glm::vec2& screenSize) {
    // Set color.
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color);
    
    RenderWidgets(screenSize);
}

void VerticalLayout::AddWidget(Widget* widget) {
    Container::AddWidget(widget);
    widget->SetPosition(GetPosition() + nextPosition);
    nextPosition.y += widget->GetSize().y;
}

void VerticalLayout::ClearWidgets() {
    Container::ClearWidgets();
    nextPosition = glm::vec2(0.f, 0.f);
}

glm::vec2 VerticalLayout::GetSize() const {
    return this->size;
}

void VerticalLayout::SetSize(const glm::vec2& size) {
    this->size = size;
}
