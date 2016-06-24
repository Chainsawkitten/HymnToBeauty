#pragma once

class Scene;

/// %Entity containing various components.
class Entity {
    public:
        /// Create new entity.
        /**
         * @param scene The scene in which the entity is contained.
         */
        Entity(Scene* scene);
        
        /// Destructor.
        ~Entity();
        
    private:
        Scene* scene;
};
