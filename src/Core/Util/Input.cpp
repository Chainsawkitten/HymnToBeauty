#include "Input.hpp"

InputHandler* InputHandler::activeInstance = nullptr;

InputHandler::InputHandler(GLFWwindow *window) {
    this->window = window;
    
    // Init mouse state.
    for (int i=0; i<3; i++) {
        mouseState[i] = false;
        mouseStateLast[i] = false;
    }
}

InputHandler* InputHandler::GetActiveInstance() {
    return activeInstance;
}

void InputHandler::SetActive() {
    activeInstance = this;
}

void InputHandler::Update() {
    // Update mouse.
    for (int i=0; i<3; i++) {
        mouseStateLast[i] = mouseState[i];
        mouseState[i] = (glfwGetMouseButton(window, i) == GLFW_PRESS);
    }
    
    glfwGetCursorPos(window, &cursorX, &cursorY);
}

bool InputHandler::MousePressed(int button) const {
    return mouseState[button] && !mouseStateLast[button];
}

bool InputHandler::MouseDown(int button) const {
    return mouseState[button];
}

bool InputHandler::MouseReleased(int button) const {
    return !mouseState[button] && mouseStateLast[button];
}

double InputHandler::CursorX() const {
    return cursorX;
}

double InputHandler::CursorY() const {
    return cursorY;
}

InputHandler* Input() {
    return InputHandler::GetActiveInstance();
}
