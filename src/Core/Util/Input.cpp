#include "Input.hpp"

InputHandler* InputHandler::activeInstance = nullptr;

InputHandler::InputHandler(GLFWwindow *window) {
    this->window = window;
}

InputHandler* InputHandler::GetActiveInstance() {
    return activeInstance;
}

void InputHandler::SetActive() {
    activeInstance = this;
}

InputHandler* Input() {
    return InputHandler::GetActiveInstance();
}
