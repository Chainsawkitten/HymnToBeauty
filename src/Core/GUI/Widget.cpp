#include "Widget.hpp"

namespace GUI {
    Widget::Widget(Widget* parent) {
        this->parent = parent;
    }
    
    Widget::~Widget() {
        
    }
    
    Widget* Widget::Parent() const {
        return parent;
    }
}
