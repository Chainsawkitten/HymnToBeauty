#pragma once

#include <GLFW/glfw3.h>
#include <json/json.h>
#include <vector>

class Input {

    public:

    ///The information needed to identify a button.
    struct Button {

        ///A string describing the action, e.g "Fire".
        char action[255];
        ///The GLFW key to press.
        int key;
        ///The GLFW state of that key.
        int state;
        ///Is the button registered in the engine.
        bool registered = false;

    };
    ///The buttons to register.
    std::vector<Button*> buttons;
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

    /// Check if a button was activated this frame.
    /**
     * @param index The index of the button in the buttons array.
     */
    bool CheckButton(int index) const;

    ///Saves the buttons to a Json value.
    /**
     * @return The saved json value.
     */
    Json::Value Save() const;

    ///Saves the buttons to a Json value.
    /**
     * @param buttonsNode the json value to load.
     */
    void Load(const Json::Value& buttonsNode);

    private:

    GLFWwindow* window;

    Input() {}

    Input(Input const&);
    void operator=(Input const&);

};
