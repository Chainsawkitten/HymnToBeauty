#include "InputManager.hpp"

#include <Utility/Log.hpp>
#include <Utility/Window.hpp>
#include <map>

#if !__ANDROID__
#include <GLFW/glfw3.h>

static std::map<GLFWwindow*, InputManager*> inputMap;

static void characterCallback(GLFWwindow* window, unsigned int codePoint) {
    inputMap[window]->CharacterCallback(codePoint);
}

static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    inputMap[window]->ScrollCallback(yOffset);
}
#endif

InputManager::InputManager(Utility::Window* window) {
    for (int i = 0; i < BUTTONS; i++) {
        buttonData[i].down = false;
        buttonData[i].released = false;
        buttonData[i].triggered = false;
    }

#if __ANDROID__
    Log(Log::ERR) << "InputManager has not been implemented for Android. No input will be available.\n";
#else
    glfwWindow = window->GetGLFWWindow();

    // Init mouse state.
    glfwSetScrollCallback(glfwWindow, scrollCallback);

    glfwSetCharCallback(glfwWindow, characterCallback);
    inputMap[glfwWindow] = this;
#endif

    Update();
}

InputManager::~InputManager() {

}

void InputManager::Update() {
#if !__ANDROID__
    glfwPollEvents();

    lastScroll = scroll;
    scroll = 0.0;

    // Get button states.
    bool values[BUTTONS] = {};
    for (Binding binding : bindings) {
        bool value = false;
        switch (binding.device) {
        case KEYBOARD:
            if (glfwGetKey(glfwWindow, binding.index) == GLFW_PRESS)
                value = true;
            break;
        case MOUSE:
            if (glfwGetMouseButton(glfwWindow, binding.index) == GLFW_PRESS)
                value = true;
            break;
        default:
            break;
        }

        if (!values[binding.button])
            values[binding.button] = value;
    }

    // Update triggered and released.
    for (int button = 0; button < BUTTONS; button++) {
        buttonData[button].triggered = !buttonData[button].down && values[button];
        buttonData[button].released = buttonData[button].down && !values[button];
        buttonData[button].down = values[button];
    }

    glfwGetCursorPos(glfwWindow, &cursorX, &cursorY);

    text = tempText;
    tempText = "";
#endif
}

double InputManager::GetCursorX() const {
    return cursorX;
}

double InputManager::GetCursorY() const {
    return cursorY;
}

glm::vec2 InputManager::GetCursorXY() const {
    return glm::vec2(cursorX, cursorY);
}

bool InputManager::GetScrollUp() const {
    return lastScroll > 0.0;
}

bool InputManager::GetScrollDown() const {
    return lastScroll < 0.0;
}

void InputManager::AssignButton(Button button, Device device, int index) {
    Binding binding;
    binding.button = button;
    binding.device = device;
    binding.index = index;

    bindings.push_back(binding);
}

bool InputManager::Pressed(Button button) const {
    return buttonData[button].down;
}

bool InputManager::Triggered(Button button) const {
    return buttonData[button].triggered;
}

void InputManager::CharacterCallback(unsigned int codePoint) {
    tempText += static_cast<char>(codePoint);
}

void InputManager::ScrollCallback(double yOffset) {
    scroll += yOffset;
}
