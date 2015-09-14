#include "Container.hpp"

namespace GUI {
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
    
    void Container::UpdateWidgets() {
        for (Widget* widget : widgets) {
            if (widget->Visible())
                widget->Update();
        }
    }
    
    void Container::RenderWidgets(int screenWidth, int screenHeight) {
        for (Widget* widget : widgets) {
            if (widget->Visible())
                widget->Render(screenWidth, screenHeight);
        }
    }
}
