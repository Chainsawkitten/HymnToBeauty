#pragma once

#include <GLFW\glfw3.h>
#include <vector>

class Input {

public:

	///The information needed to identify a button.
	struct Button {

		///A string describing the action, e.g "Fire".
		std::string action;
		///The GLFW key to press.
		int key;
		///The GLFW state of that key.
		int state;

	};
	std::vector<Button> buttons;
	/// Gets the input singleton instance.
	/**
	 * @return The input instance.
	 */
	static Input& GetInstance() {
		static Input instance;
		return instance;
	}

    /// Sets the window to check for input against.
    /**
     * @param window The target GLFWwindow.
     */
    void SetWindow(GLFWwindow* window);

    /// Adds a button to the input system.
    /**
     * @param key The GLFW key value.
     * @param action What action was performed on the key. GLFW_Press, GLFW_Release or GLFW_Repeat.
     * @param desc A string description of the buttons function.
     */
    void AddButton(int key, int action, std::string desc = "");

    /// Checks if a button were activated this frame.
    /**
     * @param index The index of the button in the buttons array.
     */
    bool Check_Button(int index);

private:

    GLFWwindow* window;

    Input() {}

    Input(Input const&);
    void operator=(Input const&);

};