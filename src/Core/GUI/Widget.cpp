#include "Widget.hpp"

namespace GUI {
    Widget::Widget(Widget* parent) {
        this->parent = parent;
        
        position = glm::vec2(0.f, 0.f);
    }
    
    Widget::~Widget() {
        
    }
    
    Widget* Widget::Parent() const {
        return parent;
    }
    
    const glm::vec2& Widget::Position() const {
        return position;
    }
    
    void Widget::SetPosition(const glm::vec2& position) {
        this->position = position;
    }
}
