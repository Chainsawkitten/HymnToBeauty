#pragma once

class Scene;

#include <glm/glm.hpp>

/// Updates the physics of a scene.
class PhysicsManager {
    friend class Hub;
    
    public:
        /// Update scene containing entities. Moves entities and updates the physics component.
        /**
         * @param scene The scene to update.
         * @param deltaTime Time since last frame (in seconds).
         */
        void Update(Scene& scene, float deltaTime);
        
    private:
        PhysicsManager();
        ~PhysicsManager();
        PhysicsManager(PhysicsManager const&) = delete;
        void operator=(PhysicsManager const&) = delete;
        
        glm::vec3 gravity = glm::vec3(0.f, -9.82f, 0.f);
};
