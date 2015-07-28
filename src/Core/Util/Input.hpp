#ifndef INPUT_HPP
#define INPUT_HPP

#include <GLFW/glfw3.h>

/** @ingroup Core
 * @{
 */

/// Class that handles input.
class InputHandler {
    public:
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
        
        /// Get whether a mouse button was pressed.
        /**
         * @param button Mouse button to check. Either GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT or GLFW_MOUSE_BUTTON_MIDDLE.
         * @return Whether button has been pressed since last frame
         */
        bool MousePressed(int button) const;
        
        /// Get whether a mouse button is down.
        /**
         * @param button Mouse button to check. Either GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT or GLFW_MOUSE_BUTTON_MIDDLE.
         * @return Whether button is down
         */
        bool MouseDown(int button) const;
        
        /// Get whether a mouse button was released.
        /**
         * @param button Mouse button to check. Either GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_RIGHT or GLFW_MOUSE_BUTTON_MIDDLE.
         * @return Whether button has been released since last frame
         */
        bool MouseReleased(int button) const;
        
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
        
    private:
        static InputHandler* activeInstance;
        
        GLFWwindow* window;
        
        // Mouse states
        bool mouseState[3];
        bool mouseStateLast[3];
        
        double cursorX, cursorY;
};

/// Get currently active input handler.
/**
 * @return The currently active input handler or nullptr.
 */
InputHandler* Input();

/** @} */

#endif
