#pragma once

#include <functional>
#include <map>
#include <memory>

class PhysicsManager;

namespace Physics {
    class Shape;
    class TriggerObserver;

    /// Represent a trigger that checks intersections of specific rigid bodies
    /// against itself.
    class Trigger {
        friend class ::PhysicsManager;

        private:
            // Construct a trigger with world transform |transform|.
            explicit Trigger(const btTransform& transform);

            // Process observers against the trigger volume, passing the world
            // in which rigid bodies reside.
            void Process(btCollisionWorld& world);

            // Get access to a particular observer of the trigger to work with
            // it in a user-defined way. If the observer is not present, one
            // will be created.
            void ForObserver(btCollisionObject* body, const std::function<void(TriggerObserver&)>& fun);

            void SetCollisionShape(std::shared_ptr<Shape> shape);
            void SetPosition(const btVector3& position);

        private:
            std::unique_ptr<btCollisionObject> trigger = nullptr;
            std::shared_ptr<Shape> shape = nullptr;
            std::vector<std::unique_ptr<TriggerObserver>> observers;
    };
}
