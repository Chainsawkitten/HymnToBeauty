#include "HorizontalLayout.hpp"
#include <Core/Resources.hpp>

namespace GUI {
    HorizontalLayout::HorizontalLayout(Widget* parent) : Container(parent) {
        rectangle = Resources().CreateRectangle();
        
        nextPosition = glm::vec2(0.f, 0.f);
    }
    
    HorizontalLayout::~HorizontalLayout() {
        Resources().FreeRectangle();
    }
    
    void HorizontalLayout::Render(const glm::vec2& screenSize) {
        // Set color.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        rectangle->Render(Position(), size, color, screenSize);
        
        RenderWidgets(screenSize);
    }
    
    void HorizontalLayout::AddWidget(Widget* widget) {
        Container::AddWidget(widget);
        widget->SetPosition(Position() + nextPosition);
        nextPosition.x += widget->Size().x;
    }
    
    glm::vec2 HorizontalLayout::Size() const {
        return this->size;
    }
    
    void HorizontalLayout::SetSize(const glm::vec2& size) {
        this->size = size;
    }
}
