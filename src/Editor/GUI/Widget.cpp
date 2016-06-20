#include "Widget.hpp"

using namespace GUI;

Widget::Widget(Widget* parent) {
    this->parent = parent;
    
    position = glm::vec2(0.f, 0.f);
    visible = true;
}

Widget::~Widget() {
    
}

Widget* Widget::GetParent() const {
    return parent;
}

const glm::vec2& Widget::GetPosition() const {
    return position;
}

void Widget::SetPosition(const glm::vec2& position) {
    this->position = position;
}

bool Widget::IsVisible() const {
    return visible;
}

void Widget::SetVisible(bool visible) {
    this->visible = visible;
}
