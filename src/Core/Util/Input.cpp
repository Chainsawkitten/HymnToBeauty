#include "Log.hpp"
#include "Input.hpp"
#include <map>

std::map<GLFWwindow*, InputHandler*> inputMap;

void characterCallback(GLFWwindow* window, unsigned int codePoint) {
    inputMap[window]->CharacterCallback(codePoint);
}

InputHandler* InputHandler::activeInstance = nullptr;

InputHandler::InputHandler(GLFWwindow *window) {
    this->window = window;
    
    // Init mouse state.
    for (int i=0; i<3; i++) {
        mouseState[i] = false;
        mouseStateLast[i] = false;
    }
    
    glfwSetCharCallback(window, characterCallback);
    inputMap[window] = this;
    text = "";
    tempText = "";
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
    
    text = tempText;
    tempText = "";
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

const std::string& InputHandler::Text() const {
    return text;
}

void InputHandler::CharacterCallback(unsigned int codePoint) {
    tempText += static_cast<char>(codePoint);
}

InputHandler* Input() {
    return InputHandler::GetActiveInstance();
}
