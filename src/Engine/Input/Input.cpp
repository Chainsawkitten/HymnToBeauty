#include "Input.hpp"

void Input::SetWindow(GLFWwindow* window) {

    getInstance().window = window;

}

void Input::AddButton(int key, int action, std::string desc) {
    
    Button button;
    button.key = key;
    button.action = action;
    button.desc = desc;

    getInstance().buttons[getInstance().nrOfButtons] = button;

}

bool Input::Check_Button(int index){

    Button button = getInstance().buttons[index];

    int state = glfwGetKey(getInstance().window, button.key);

    return state == button.action;

}