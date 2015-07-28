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
        
    private:
        static InputHandler* activeInstance;
        
        GLFWwindow* window;
};

/// Get currently active input handler.
/**
 * @return The currently active input handler or nullptr.
 */
InputHandler* Input();

/** @} */

#endif
