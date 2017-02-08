#include "Input.hpp"

void Input::SetWindow(GLFWwindow* window) {

    window = window;

}

void Input::AddButton(int key, int state, std::string action) {
    
    Button button;
    button.key = key;
    button.state = state;
    button.action = action;

	buttons.push_back(button);

}

bool Input::Check_Button(int index){

    Button button = buttons[index];

    int state = glfwGetKey(window, button.key);

    return state == button.state;

}