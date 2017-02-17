#include "Input.hpp"

#include <cstring>

void Input::SetWindow(GLFWwindow* window) {
    this->window = window;
}

bool Input::CheckButton(int index) const{
    Button* button = buttons[index];
    int state = glfwGetKey(window, button->key);
    return state == button->state;
}

Json::Value Input::Save() const{
    Json::Value buttonsNode;
    
    buttonsNode["size"] = buttons.size();
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
