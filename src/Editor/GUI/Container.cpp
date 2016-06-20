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

void Container::AddWidget(Widget* widget) {
    widgets.push_back(widget);
}

const vector<Widget*>& Container::Widgets() const {
    return widgets;
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
