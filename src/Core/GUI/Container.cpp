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
            widget->Update();
        }
    }
    
    void Container::RenderWidgets(int screenWidth, int screenHeight) {
        for (Widget* widget : widgets) {
            widget->Render(screenWidth, screenHeight);
        }
    }
}
