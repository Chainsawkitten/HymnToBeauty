#pragma once

#include <glm/glm.hpp>

namespace Physics {
    /// A 2D rectangle.
    class Rectangle {
        public:
            /// Create new rectangle.
            /**
             * @param position The rectangle's position.
             * @param size The rectangle's size.
             */
            Rectangle(const glm::vec2& position, const glm::vec2& size);
            
            /// Check collision between the rectangle and a point.
            /**
             * @param point The point to check.
             * @return Whether there was a collision.
             */
            bool Collide(const glm::vec2& point);
            
        private:
            glm::vec2 position;
            glm::vec2 size;
    };
}
