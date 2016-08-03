#include "VerticalScrollLayout.hpp"

#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Util/Input.hpp>

using namespace GUI;

VerticalScrollLayout::VerticalScrollLayout(Widget* parent) : Container(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    
    nextPosition = glm::vec2(0.f, 0.f);
    scrollPosition = 0U;
}

VerticalScrollLayout::~VerticalScrollLayout() {
    Managers().resourceManager->FreeRectangle();
}

void VerticalScrollLayout::Update() {
    Container::Update();
    
    if (Input()->ScrollUp() && scrollPosition > 0U) {
        --scrollPosition;
        UpdatePositions();
    } else if (Input()->ScrollDown() && scrollPosition < GetWidgets().size()) {
        ++scrollPosition;
        UpdatePositions();
    }
}

void VerticalScrollLayout::Render(const glm::vec2& screenSize) {
    // Set color.
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color, screenSize);
    
    // Draw scrollbar.
    if (!GetWidgets().empty()) {
        color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
        float yScrolled = 0.f;
        float yCovered = 0.f;
        float yTotal = 0.f;
        for (std::size_t i=0U; i < GetWidgets().size(); ++i) {
            yTotal += GetWidgets()[i]->GetSize().y;
            
            if (i < scrollPosition) {
                yScrolled += GetWidgets()[i]->GetSize().y;
            } else if (yTotal - yScrolled < size.y) {
                yCovered += GetWidgets()[i]->GetSize().y;
            }
        }
        rectangle->Render(GetPosition() + glm::vec2(size.x - 20.f, size.y * yScrolled / yTotal), glm::vec2(20.f, size.y * yCovered / yTotal), color, screenSize);
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
    scrollPosition = 0U;
}

glm::vec2 VerticalScrollLayout::GetSize() const {
    return this->size;
}

void VerticalScrollLayout::SetSize(const glm::vec2& size) {
    this->size = size;
}

void VerticalScrollLayout::UpdatePositions() {
    glm::vec2 nextPosition(0.f, 0.f);
    
    for (std::size_t i=0U; i < GetWidgets().size(); ++i) {
        if (i >= scrollPosition) {
            GetWidgets()[i]->SetPosition(GetPosition() + nextPosition);
            nextPosition.y += GetWidgets()[i]->GetSize().y;
        }
    }
}
