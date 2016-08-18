#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

/// Handles the game window.
class Game {
    public:
        /// Create new game window.
        Game();

        /// Destructor.
        ~Game();

        /// Get whether the game window should close.
        /**
         * @return Whether the game window should close
         */
        bool ShouldClose() const;

        /// Update the game.
        void Update();

        /// Render the game.
        void Render();
        
        /// Get the size of the window.
        /**
         * @return The size of the render area in pixels
         */
        glm::vec2 GetSize() const;

    private:
        GLFWwindow* window;
};
