#include "Container.hpp"

using namespace GUI;
using namespace std;

Container::Container(Widget* parent) : Widget(parent) {
    
}

Container::~Container() {
    
}

void Container::Update() {
    UpdateWidgets();
}

const vector<Widget*>& Container::Widgets() const {
    return widgets;
}

void Container::AddWidget(Widget* widget) {
    widgets.push_back(widget);
}

void Container::SetPosition(const glm::vec2& position) {
    glm::vec2 offset = position - Position();
    
    Widget::SetPosition(position);
    
    for (Widget* widget : Widgets())
        widget->SetPosition(widget->Position() + offset);
}

void Container::UpdateWidgets() {
    for (Widget* widget : widgets) {
        if (widget->Visible())
            widget->Update();
    }
}

void Container::RenderWidgets(const glm::vec2& screenSize) {
    for (Widget* widget : widgets) {
        if (widget->Visible())
            widget->Render(screenSize);
    }
}
