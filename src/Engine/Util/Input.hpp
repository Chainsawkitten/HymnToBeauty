#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

/// Class that handles input.
class InputHandler {
    public:
        /// Button codes.
        enum Button {
            CLICK = 0, ///< Clicking in the editor.
            BACK, ///< Erase previous character.
            ERASE, ///< Erase next character.
            LEFT, ///< Move text input marker left.
            RIGHT, ///< Move text input marker right.
            HOME, ///< Move text input marker to beginning of text field.
            END, ///< Move text input marker to end of text field.
            BUTTONS, ///< Total number of inputs
        };
        
        /// Input device codes
        enum Device {
            KEYBOARD = 0, ///< Keyboard input
            MOUSE, ///< Mouse buttons
            INPUT_DEVICES, ///< Number of input devices
        };
        
        /// Create new input handler.
        /**
         * @param window %Window to get input for.
         */
        InputHandler(GLFWwindow* window);
        
        /// Get currently active input handler.
        /**
         * @return The currently active input handler or nullptr.
         */
        static InputHandler* GetActiveInstance();
        
        /// Set as currently active input handler.
        void SetActive();
        
        /// Update input state.
        void Update();
        
        /// Get cursor's horizontal position.
        /**
         * @return X-position of the cursor
         */
        double CursorX() const;
        
        /// Get cursor's vertical position.
        /**
         * @return Y-position of the cursor
         */
        double CursorY() const;
        
        /// Get whether user has moved scroll wheel up.
        /**
         * @return Whether user has scrolled up
         */
        bool ScrollUp() const;
        
        /// Get whether user has moved scroll wheel down.
        /**
         * @return Whether user has scrolled down
         */
        bool ScrollDown() const;
        
        /// Assign a button binding.
        /**
         * See <a href="http://www.glfw.org/docs/latest/group__keys.html">GLFW keyboard documentation</a> for indices for keys.
         * For mouse buttons, either GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT or GLFW_MOUSE_BUTTON_MIDDLE.
         * @param button Which button to bind.
         * @param device Which device (KEYBOARD or MOUSE).
         * @param index Index of the key.
         */
        void AssignButton(Button button, Device device, int index);
        
        /// Gets whether a button is currently down.
        /**
         * @param button The button to check.
         * @return Whether the button is down
         */
        bool Pressed(Button button);
        
        /// Gets whether a button was just pressed.
        /**
         * Checks whether a button was pressed between the last two calls to update().
         * @param button The button to check.
         * @return Whether the button was pressed
         */
        bool Triggered(Button button);
        
        /// Gets whether a button was just released.
        /**
         * Checks whether a button was released between the last two calls to update().
         * @param button The button to check.
         * @return Whether the button was released
         */
        bool Released(Button button);
        
        /// Get text input since last frame.
        /**
         * @return Text input since last frame.
         */
        const std::string& Text() const;
        
        /// GLFW character callback.
        /**
         * @param codePoint Unicode code point.
         */
        void CharacterCallback(unsigned int codePoint);
        
        /// GLFW scrolling callback.
        /**
         * @param yoffset Offset along the Y-axis.
         */
        void ScrollCallback(double yOffset);
        
    private:
        static InputHandler* activeInstance;
        
        GLFWwindow* window;
        
        // Bindings
        struct Binding {
            Button button;
            Device device;
            int index;
        };
        std::vector<Binding> bindings;
        
        // Data
        struct ButtonData {
            bool down = false;
            bool released = false;
            bool triggered = false;
        };
        
        // Button data.
        ButtonData buttonData[BUTTONS];
        
        double cursorX, cursorY;
        double lastScroll = 0.0;
        double scroll = 0.0;
        
        std::string text = "", tempText = "";
};

/// Get currently active input handler.
/**
 * @return The currently active input handler or nullptr.
 */
InputHandler* Input();
