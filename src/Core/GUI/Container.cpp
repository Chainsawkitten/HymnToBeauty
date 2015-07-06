#include "Container.hpp"

namespace GUI {
    Container::Container(Widget* parent) : Widget(parent) {
        
    }
    
    Container::~Container() {
        
    }
    
    void Container::Update(GLFWwindow* window) {
        UpdateWidgets(window);
    }
    
    void Container::AddWidget(Widget* widget) {
        widgets.push_back(widget);
    }
    
    void Container::UpdateWidgets(GLFWwindow* window) {
        for (Widget* widget : widgets) {
            widget->Update(window);
        }
    }
    
    void Container::RenderWidgets(int screenWidth, int screenHeight) {
        for (Widget* widget : widgets) {
            widget->Render(screenWidth, screenHeight);
        }
    }
}
