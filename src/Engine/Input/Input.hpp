#pragma once

#include <json/json.h>
#include <vector>

namespace Utility {
class Window;
}

#if !__ANDROID__
struct GLFWwindow;
#endif

/// Hold information about the hymn's inputs.
/// @todo Android
class Input {
  public:
    /// The information needed to identify a button.
    struct Button {
        /// A string describing the action, e.g "Fire".
        char action[255];

        /// The GLFW key to press.
        int key;

        /// The GLFW state of that key.
        int state;
    };

    /// The buttons to register.
    std::vector<Button*> buttons;

    /// Get the input singleton instance.
    /**
     * @return The input instance.
     */
    static Input& GetInstance();

    /// Set the window to check for input against.
    /**
     * @param window The window to get input from.
     */
    void SetWindow(Utility::Window* window);

    /// Check if a button was activated this frame.
    /**
     * @param index The index of the button in the buttons array.
     * @return Whether the button was activated this frame.
     */
    bool CheckButton(int index) const;

    /// Save the buttons to a JSON value.
    /**
     * @return The saved JSON value.
     */
    Json::Value Save() const;

    /// Load buttons from JSON node.
    /**
     * @param buttonsNode The JSON value to load.
     */
    void Load(const Json::Value& buttonsNode);

  private:
#if !__ANDROID__
    GLFWwindow* window;
#endif

    Input() {}

    Input(Input const&) = delete;
    void operator=(Input const&) = delete;
};
