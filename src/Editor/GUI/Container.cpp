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

const vector<Widget*>& Container::GetWidgets() const {
    return widgets;
}

void Container::AddWidget(Widget* widget) {
    widgets.push_back(widget);
}

void Container::SetPosition(const glm::vec2& position) {
    glm::vec2 offset = position - GetPosition();
    
    Widget::SetPosition(position);
    
    for (Widget* widget : widgets)
        widget->SetPosition(widget->GetPosition() + offset);
}

void Container::UpdateWidgets() {
    for (Widget* widget : widgets) {
        if (widget->IsVisible())
            widget->Update();
    }
}

void Container::RenderWidgets(const glm::vec2& screenSize) {
    for (Widget* widget : widgets) {
        if (widget->IsVisible())
            widget->Render(screenSize);
    }
}
