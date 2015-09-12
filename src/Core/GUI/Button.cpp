#include "Button.hpp"
#include "../Util/Input.hpp"

namespace GUI {
    Button::Button(Widget* parent) : Widget(parent) {
        mouseHover = false;
        hasClickedCallback = false;
        size = glm::vec2(64.f, 64.f);
    }
    
    Button::~Button() {
        
    }
    
    void Button::Update() {
        double xpos = Input()->CursorX();
        double ypos = Input()->CursorY();
        
        mouseHover = xpos >= Position().x && xpos < Position().x + size.x && ypos >= Position().y && ypos < Position().y + size.y;
        
        if (mouseHover && Input()->MousePressed(GLFW_MOUSE_BUTTON_LEFT) && hasClickedCallback) {
            clickedCallback();
        }
    }
    
    glm::vec2 Button::Size() const {
        return size;
    }
    
    void Button::SetSize(const glm::vec2& size) {
        this->size = size;
    }
    
    void Button::SetClickedCallback(std::function<void()> callback) {
        clickedCallback = callback;
        hasClickedCallback = true;
    }
    
    bool Button::MouseHover() const {
        return mouseHover;
    }
}
