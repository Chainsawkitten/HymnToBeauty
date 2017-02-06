#pragma once

#include <GLFW\glfw3.h>
#include <vector>

class Input {

public:

    /// Sets the window to check for input against.
    /**
    * @param window The target GLFWwindow.
    */
    static void SetWindow(GLFWwindow* window);

    /// Adds a button to the input system.
    /**
    * @param key The GLFW key value.
    * @param action What action was performed on the key. GLFW_Press, GLFW_Release or GLFW_Repeat.
    * @param desc A string description of the buttons function.
    */
    static void AddButton(int key, int state, const std::string& action);

	/// Checks if a button were activated this frame.
	/**
	* @param index The index of the button in the buttons array.
	*/
	static bool Check_Button(int key, int action);

	/// Checks if a button were activated this frame.
	/**
	* @param index The index of the button in the buttons array.
	*/
	static bool Check_Button(const std::string& action);

private:

    static Input& getInstance() {

        static Input instance;
        return instance;

    }

    GLFWwindow* window;

    struct Button {

        std::string action;
        int key;
        int state;

    };

    int nrOfButtons;
    int size;
    Button* buttons;

    Input() {

		buttons = new Button[10];
        nrOfButtons = 0;
        size = 10;

    }
    Input(Input const&);
    void operator=(Input const&);

};





