#include "Input.hpp"

#include "Log.hpp"
#include <map>
#include <GLFW/glfw3.h>

std::map<GLFWwindow*, InputHandler*> inputMap;

void characterCallback(GLFWwindow* window, unsigned int codePoint) {
    inputMap[window]->CharacterCallback(codePoint);
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    inputMap[window]->ScrollCallback(yOffset);
}

InputHandler* InputHandler::activeInstance = nullptr;

InputHandler::InputHandler(GLFWwindow *window) {
    this->window = window;
    
    // Init mouse state.
    glfwSetScrollCallback(window, scrollCallback);
    
    glfwSetCharCallback(window, characterCallback);
    inputMap[window] = this;
}

InputHandler* InputHandler::GetActiveInstance() {
    return activeInstance;
}

void InputHandler::SetActive() {
    activeInstance = this;
}

void InputHandler::Update() {
    lastScroll = scroll;
    scroll = 0.0;
    
    // Get button states.
    bool values[BUTTONS] = {};
    for (Binding binding : bindings) {
        bool value = false;
        switch (binding.device) {
        case KEYBOARD:
            if (glfwGetKey(window, binding.index) == GLFW_PRESS)
                value = true;
            break;
        case MOUSE:
            if (glfwGetMouseButton(window, binding.index) == GLFW_PRESS)
                value = true;
            break;
        default:
            break;
        }
        
        if (!values[binding.button])
            values[binding.button] = value;
    }
    
    // Update triggered and released.
    for (int button=0; button<BUTTONS; button++) {
        buttonData[button].triggered = !buttonData[button].down && values[button];
        buttonData[button].released = buttonData[button].down && !values[button];
        buttonData[button].down = values[button];
    }
    
    glfwGetCursorPos(window, &cursorX, &cursorY);
    
    text = tempText;
    tempText = "";
}

double InputHandler::CursorX() const {
    return cursorX;
}

double InputHandler::CursorY() const {
    return cursorY;
}

void InputHandler::CenterMouse() {
    cursorX = 0.0;
    cursorY = 0.0;
    glfwSetCursorPos(window, cursorX, cursorY);
}

bool InputHandler::ScrollUp() const {
    return lastScroll > 0.0;
}

bool InputHandler::ScrollDown() const {
    return lastScroll < 0.0;
}

void InputHandler::AssignButton(Button button, Device device, int index) {
    Binding binding;
    binding.button = button;
    binding.device = device;
    binding.index = index;
    
    bindings.push_back(binding);
}

bool InputHandler::Pressed(Button button) {
    return buttonData[button].down;
}

bool InputHandler::Triggered(Button button) {
    return buttonData[button].triggered;
}

bool InputHandler::Released(Button button) {
    return buttonData[button].released;
}

const std::string& InputHandler::Text() const {
    return text;
}

void InputHandler::CharacterCallback(unsigned int codePoint) {
    tempText += static_cast<char>(codePoint);
}

void InputHandler::ScrollCallback(double yOffset) {
    scroll += yOffset;
}

InputHandler* Input() {
    return InputHandler::GetActiveInstance();
}
