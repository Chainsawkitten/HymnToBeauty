#include "Input.hpp"

#include <cstring>
#include <Utility/Log.hpp>
#include <Utility/Window.hpp>

#if !__ANDROID__
#include <GLFW/glfw3.h>
#endif

Input& Input::GetInstance() {
    static Input instance;
    return instance;
}

void Input::SetWindow(Utility::Window* window) {
#if !__ANDROID__
    this->window = window->GetGLFWWindow();
#endif
}

bool Input::CheckButton(int index) const {
#if __ANDROID__
    Log(Log::ERR) << "Input::CheckButton not implemented for Android.";
    return 0;
#else
    Button* button = buttons[index];
    int state;
    if (button->key == 420)
        state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
    else
        state = glfwGetKey(window, button->key);
    return state == button->state;
#endif
}

Json::Value Input::Save() const {
    Json::Value buttonsNode;

    buttonsNode["size"] = static_cast<Json::UInt>(buttons.size());
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        buttonsNode[std::to_string(i)]["action"] = buttons[i]->action;
        buttonsNode[std::to_string(i)]["key"] = buttons[i]->key;
        buttonsNode[std::to_string(i)]["state"] = buttons[i]->state;
    }

    return buttonsNode;
}

void Input::Load(const Json::Value& buttonsNode) {
    buttons.clear();

    unsigned int size = buttonsNode.get("size", 0).asUInt();
    for (unsigned int i = 0; i < size; ++i) {
        Button* button = new Button();

        strcpy(button->action, buttonsNode[std::to_string(i)]["action"].asCString());
        button->key = buttonsNode[std::to_string(i)]["key"].asInt();
        button->state = buttonsNode[std::to_string(i)]["state"].asInt();

        buttons.push_back(button);
    }
}
