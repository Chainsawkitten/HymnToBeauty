#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <Utility/LockBox.hpp>
#include <vector>
#include "../Entity/ComponentContainer.hpp"

namespace Component {
    class RigidBody;
    class Shape;
}

namespace Physics {
    class Shape;
    class Trigger;
}

namespace Json {
    class Value;
}

class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class Entity;

/// Updates the physics of the world.
class PhysicsManager {
    friend class Hub;

    public:
        /// Moves entities and updates the physics component.
        /**
         * @param deltaTime Time since last frame (in seconds).
         */
        void Update(float deltaTime);

        /// Update transforms of entities according to positions of physics
        /// components.
        void UpdateEntityTransforms();

        /// Set up listener for when |object| has entered |trigger|.
        /**
         * @param trigger What trigger to check against.
         * @param object Body that is to enter the trigger volume.
         * @param callback Function to call when resolving event.
         */
        void OnTriggerEnter(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback);

        /// Stop listening for when |object| has entered |trigger|.
        /**
         * @param trigger What trigger to stop listening on.
         * @param object Body that is to be forgotten.
         */
        void ForgetTriggerEnter(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object);

        /// Set up listener for when |object| is intersecting |trigger|.
        /**
         * @param trigger What trigger to check against.
         * @param object Body that is to cause trigger to fire.
         * @param callback Function to call when resolving event.
         */
        void OnTriggerRetain(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback);

        /// Stop listening for when |object| is intersecting |trigger|.
        /**
         * @param trigger What trigger to stop listening on.
         * @param object Body that is to be forgotten.
         */
        void ForgetTriggerRetain(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object);

        /// Set up listener for when |object| has left |trigger|.
        /**
         * @param trigger What trigger to check against.
         * @param object Body that is to cause trigger to fire.
         * @param callback Function to call when resolving event.
         */
        void OnTriggerLeave(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback);

        /// Stop listening for when |object| has left |trigger|.
        /**
         * @param trigger What trigger to stop listening on.
         * @param object Body that is to be forgotten.
         */
        void ForgetTriggerLeave(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object);

        /// Create rigid body component.
        /**
         * @param owner The %Entity that will own the component.
         * @return The created component.
         */
        Component::RigidBody* CreateRigidBody(Entity* owner);

        /// Create rigid body component.
        /**
         * @param owner The %Entity that will own the component.
         * @param node Json node from which to load component definition.
         * @return The created component.
         */
        Component::RigidBody* CreateRigidBody(Entity* owner, const Json::Value& node);

        /// Create a component that represents a physical shape.
        /**
         * @param owner The %Entity that will own the component.
         * @return The created component.
         */
        Component::Shape* CreateShape(Entity* owner);

        /// Create a component that represents a physical shape.
        /**
         * @param owner The %Entity that will own the component.
         * @param node Json node from which to load component definition.
         * @return The created component.
         */
        Component::Shape* CreateShape(Entity* owner, const Json::Value& node);

        /// Create a trigger volume that can be used to check intersection
        /// events against physics bodies.
        /**
         * @param shape Shape of the trigger volume.
         * @return A reference to the internal trigger.
         */
        Utility::LockBox<Physics::Trigger> CreateTrigger(std::shared_ptr<Physics::Shape> shape);

        /// Inform the physics manager that a given trigger volume will no
        /// longer be used so that it can be freed from memory.
        /**
         * @param trigger Trigger volume to release.
         */
        void ReleaseTriggerVolume(Utility::LockBox<Physics::Trigger>&& trigger);

        /// Set the position of a trigger volume.
        /**
         * @param trigger Volume to reposition.
         * @param position New position in world space.
         */
        void SetPosition(Utility::LockBox<Physics::Trigger> trigger, const glm::vec3& position);

        /// Set the shape of a given Component::Shape component.
        /**
         * @param comp The component on which to set the shape.
         * @param shape A Physics::Shape object that holds the shape definition.
         */
        void SetShape(Component::Shape* comp, std::shared_ptr<::Physics::Shape> shape);

        /// Get the mass of a rigid body component.
        /**
         * @param comp Rigid body component to get mass of.
         * @return Mass in kilograms.
         */
        float GetMass(Component::RigidBody* comp);

        /// Set the volume shape of a trigger.
        /**
         * @param trigger Trigger to alter shape of.
         * @param shape Shape definition.
         */
        void SetShape(Utility::LockBox<Physics::Trigger> trigger, std::shared_ptr<Physics::Shape> shape);

        /// Set the mass of a Component::RigidBody component.
        /**
         * @param comp The component on which to set mass.
         * @param mass Mass in kilograms.
         */
        void SetMass(Component::RigidBody* comp, float mass);

        /// Set the friction coefficient of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param friction Friction coefficient.
         */
        void SetFriction(Component::RigidBody* comp, float friction);

        /// Set the rolling friction coefficient of a Component::RigidBody
        /// component.
        /**
         * @param comp Rigid body to alter.
         * @param friction Friction coefficient.
         */
        void SetRollingFriction(Component::RigidBody* comp, float friction);

        /// Set the spinning friction coefficient of a Component::RigidBody
        /// component.
        /**
         * @param comp Rigid body to alter.
         * @param friction Friction coefficient.
         */
        void SetSpinningFriction(Component::RigidBody* comp, float friction);

        /// Set the restitution (bounciness) of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param cor Coefficient of restitution.
         */
        void SetRestitution(Component::RigidBody* comp, float cor);

        /// Set the linear damping factor of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param damping Linear damping.
         */
        void SetLinearDamping(Component::RigidBody* comp, float damping);

        /// Set the angular damping factor of a Component::RigidBody component.
        /**
         * @param comp Rigid body to alter.
         * @param damping Angular damping.
         */
        void SetAngularDamping(Component::RigidBody* comp, float damping);

        /// Turn a rigid body into a kinematic object, putting movement in the
        /// control of the programmer.
        /**
         * @param comp Rigid body to make kinematic.
         */
        void MakeKinematic(Component::RigidBody* comp);

        /// Turn a rigid body into a dynamic object.
        /**
         * @param comp Rigid body to make dynamic.
         */
        void MakeDynamic(Component::RigidBody* comp);

        /// Enables/disables ghost functionality on a rigid body.
        /**
         * @param comp Rigid body to alter state of.
         * @param ghost True: makes the rigid object a ghost, disregarding all
         * collisions. False: disables ghost state, reverting to kinematic or
         * dynamic as before.
         */
        void SetGhost(Component::RigidBody* comp, bool ghost);

        /// Forces a dynamic rigid body to synchronize its transform with that
        /// of its owning entity during the next simulation iteration.
        /**
         * @param comp Rigid body to synchronize.
         */
        void ForceTransformSync(Component::RigidBody* comp);

        /// Halts movement of a kinematic rigid body.
        /**
         * @param comp Rigid body to halt.
         */
        void HaltMovement(Component::RigidBody* comp);

        /// Get all shape components.
        /**
         * @return All shape components.
         */
        const std::vector<Component::Shape*>& GetShapeComponents() const;

        /// Remove all killed components.
        void ClearKilledComponents();

    private:
        PhysicsManager();
        ~PhysicsManager();
        PhysicsManager(PhysicsManager const&) = delete;
        void operator=(PhysicsManager const&) = delete;

        glm::vec3 gravity = glm::vec3(0.f, -9.82f, 0.f);

        ComponentContainer<Component::RigidBody> rigidBodyComponents;
        ComponentContainer<Component::Shape> shapeComponents;

        btBroadphaseInterface* broadphase = nullptr;
        btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
        btCollisionDispatcher* dispatcher = nullptr;
        btSequentialImpulseConstraintSolver* solver = nullptr;
        btDiscreteDynamicsWorld* dynamicsWorld = nullptr;

        std::shared_ptr<Utility::LockBox<Physics::Trigger>::Key> triggerLockBoxKey;
        std::vector<::Physics::Trigger*> triggers;
};
