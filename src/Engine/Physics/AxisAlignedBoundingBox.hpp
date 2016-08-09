#pragma once

#include <glm/glm.hpp>

namespace Physics {
    /// An axis-aligned bounding box.
    /**
     * Used for intersection testing.
     */
    class AxisAlignedBoundingBox {
        public:
            /// Create new axis-aligned bounding box.
            /**
             * @param dimensions Dimensions.
             * @param origin Origin.
             * @param minVertex Min vertex.
             * @param maxVertex Max vertex.
             */
            AxisAlignedBoundingBox(const glm::vec3& dimensions = { 0.f, 0.f, 0.f }, const glm::vec3& origin = { 0.f, 0.f, 0.f }, const glm::vec3& minVertex = { 0.f, 0.f, 0.f }, const glm::vec3& maxVertex = { 0.f, 0.f, 0.f });
            
            /// Dimensions.
            glm::vec3 dimensions;
            
            /// Origin.
            glm::vec3 origin;
            
            /// Min vertex.
            glm::vec3 minVertex;
            
            /// Max vertex.
            glm::vec3 maxVertex;
    };
}
