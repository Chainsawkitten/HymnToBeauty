#include "Button.hpp"
#include <Engine/Util/Input.hpp>

using namespace GUI;

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
    
    mouseHover = xpos >= GetPosition().x && xpos < GetPosition().x + size.x && ypos >= GetPosition().y && ypos < GetPosition().y + size.y;
    
    if (mouseHover && Input()->MousePressed(GLFW_MOUSE_BUTTON_LEFT) && hasClickedCallback) {
        clickedCallback();
    }
}

glm::vec2 Button::GetSize() const {
    return size;
}

void Button::SetSize(const glm::vec2& size) {
    this->size = size;
}

void Button::SetClickedCallback(std::function<void()> callback) {
    clickedCallback = callback;
    hasClickedCallback = true;
}

bool Button::GetMouseHover() const {
    return mouseHover;
}
