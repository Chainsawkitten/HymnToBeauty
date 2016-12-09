#include "Input.hpp"

void Input::AddButton(int key, int action, std::string desc = "") {

    buttons.push_back(Button(key, action, desc));

}

bool Input::Check_Button(int index) {

    Button button = buttons[index];

    int state = glfwGetKey(window, button.key);

    return state == button.action;
    

}