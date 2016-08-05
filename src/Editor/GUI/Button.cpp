#include "Button.hpp"

#include <Engine/Util/Input.hpp>
#include <Engine/Physics/Rectangle.hpp>

using namespace GUI;

Button::Button(Widget* parent) : Widget(parent) {
    
}

Button::~Button() {
    
}

void Button::Update() {
    glm::vec2 mousePosition(Input()->CursorX(), Input()->CursorY());
    Physics::Rectangle rect(GetPosition(), size);
    mouseHover = rect.Collide(mousePosition);
    
    if (mouseHover && Input()->Triggered(InputHandler::CLICK) && hasClickedCallback) {
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
