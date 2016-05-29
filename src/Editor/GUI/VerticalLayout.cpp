#include "VerticalLayout.hpp"
#include <Engine/Resources.hpp>

namespace GUI {
    VerticalLayout::VerticalLayout(Widget* parent) : Container(parent) {
        rectangle = Resources().CreateRectangle();
        
        nextPosition = glm::vec2(0.f, 0.f);
    }
    
    VerticalLayout::~VerticalLayout() {
        Resources().FreeRectangle();
    }
    
    void VerticalLayout::Render(const glm::vec2& screenSize) {
        // Set color.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        rectangle->Render(Position(), size, color, screenSize);
        
        RenderWidgets(screenSize);
    }
    
    void VerticalLayout::AddWidget(Widget* widget) {
        Container::AddWidget(widget);
        widget->SetPosition(Position() + nextPosition);
        nextPosition.y += widget->Size().y;
    }
    
    glm::vec2 VerticalLayout::Size() const {
        return this->size;
    }
    
    void VerticalLayout::SetSize(const glm::vec2& size) {
        this->size = size;
    }
}
