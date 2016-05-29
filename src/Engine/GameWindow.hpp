#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <GLFW/glfw3.h>

/** @ingroup Core
 * @{
 */

/// Handles the game window.
class GameWindow {
    public:
        /// Create new game window.
        GameWindow();

        /// Destructor.
        ~GameWindow();

        /// Get whether the game window should close.
        /**
         * @return Whether the game window should close
         */
        bool ShouldClose() const;

        /// Update the game.
        void Update();

        /// Render the game.
        void Render();

    private:
        GLFWwindow* window;
};

/** @} */

#endif
