#include "VerticalScrollLayout.hpp"

#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>

using namespace GUI;

VerticalScrollLayout::VerticalScrollLayout(Widget* parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    nextPosition = glm::vec2(0.f, 0.f);
}

VerticalScrollLayout::~VerticalScrollLayout() {
    Managers().resourceManager->FreeRectangle();
}

void VerticalScrollLayout::Render(const glm::vec2& screenSize) {
    // Set color.
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    // Draw scrollbar.
    if (!GetWidgets().empty()) {
        color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
        float yCovered = 0.f;
        float yTotal = 0.f;
        for (Widget* widget : GetWidgets()) {
            yTotal += widget->GetSize().y;
            if (yTotal < size.y)
                yCovered = yTotal;
        }
        rectangle->Render(GetPosition() + glm::vec2(size.x - 20.f, 0.f), glm::vec2(20.f, size.y * yCovered / yTotal), color, screenSize);
    }
    
    RenderWidgets(screenSize);
}

void VerticalScrollLayout::AddWidget(Widget* widget) {
    Container::AddWidget(widget);
    widget->SetPosition(GetPosition() + nextPosition);
    nextPosition.y += widget->GetSize().y;
}

void VerticalScrollLayout::ClearWidgets() {
    Container::ClearWidgets();
    nextPosition = glm::vec2(0.f, 0.f);
}

glm::vec2 VerticalScrollLayout::GetSize() const {
    return this->size;
}

void VerticalScrollLayout::SetSize(const glm::vec2& size) {
    this->size = size;
}
