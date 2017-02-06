#include "Input.hpp"

void Input::SetWindow(GLFWwindow* window) {

    getInstance().window = window;

}

void Input::AddButton(int key, int state, const std::string& action) {
    
	//if (getInstance().nrOfButtons >= getInstance().size) {

	//	Button* new_buttons = new Input::Button[getInstance().size + 10];

	//	for (int i = 0; i < getInstance().size; i++) {

	//		new_buttons[i] = getInstance().buttons[i];

	//	}
	//	getInstance().size += 10;
	//	getInstance().buttons = new_buttons;

	//}

	Button button;
	button.key = key;
	button.state = state;
	button.action = action;

	getInstance().buttons[getInstance().nrOfButtons] = button;
	getInstance().nrOfButtons++;

}

bool Input::Check_Button(const std::string& action) {

	Button* buttons = getInstance().buttons;

	for (int i = 0; i < getInstance().size; i++) {

		if (action == buttons[i].action) {

			int state = glfwGetKey(getInstance().window, buttons[i].key);

			return state == buttons[i].state;
		}

	}

	return false;

}

bool Input::Check_Button(int key, int action) {

	int state = glfwGetKey(getInstance().window, key);

	return state == action;

}