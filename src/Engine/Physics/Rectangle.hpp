#pragma once

#include <glm/glm.hpp>

namespace Physics {
    /// A 2D rectangle.
    class Rectangle {
        public:
            /// Create new rectangle.
            /**
             * @param position Position.
             * @param size Size.
             */
            Rectangle(const glm::vec3& position, const glm::vec3& size);
            
        private:
            glm::vec3 position;
            glm::vec3 size;
    };
}
