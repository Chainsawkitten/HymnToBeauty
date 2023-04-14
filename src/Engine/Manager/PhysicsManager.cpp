#include "PhysicsManager.hpp"

#include <algorithm>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../Component/RigidBody.hpp"
#include "../Component/Shape.hpp"
#include "../Entity/Entity.hpp"
#include "../Physics/GlmConversion.hpp"
#include "../Physics/Shape.hpp"
#include "../Physics/Trigger.hpp"
#include "../Physics/TriggerObserver.hpp"

PhysicsManager::PhysicsManager() {
    // The broadphase is used to quickly cull bodies that will not collide with
    // each other, normally by leveraging some simpler (and rough) test such as
    // bounding boxes.
    broadphase = new btDbvtBroadphase;

    // With the collision configuration one can configure collision detection
    // algorithms.
    collisionConfiguration = new btDefaultCollisionConfiguration;
    dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // The solver makes objects interact by making use of gravity, collisions,
    // game logic supplied forces, and constraints.
    solver = new btSequentialImpulseConstraintSolver;

    // The dynamics world encompasses objects included in the simulation.
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

    // Y axis up
    dynamicsWorld->setGravity(btVector3(0.f, -9.82f, 0.f));

    // Set the lockbox key we will use for lockboxes created in here.
    triggerLockBoxKey.reset(new Utility::LockBox<Physics::Trigger>::Key());
}

PhysicsManager::~PhysicsManager() {
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;

    for (auto t : triggers)
        delete t;
}

void PhysicsManager::Update(float deltaTime) {
    for (auto rigidBodyComp : rigidBodyComponents.GetAll()) {
        if (rigidBodyComp->IsKilled() || !rigidBodyComp->entity->IsEnabled()) {
            continue;
        }

        auto worldPos = rigidBodyComp->entity->GetWorldPosition();
        auto worldOrientation = rigidBodyComp->entity->GetWorldOrientation();
        if (rigidBodyComp->ghost) {
            rigidBodyComp->SetPosition(worldPos);
            rigidBodyComp->SetOrientation(worldOrientation);
            rigidBodyComp->ghostObject->activate(true);
        } else if (rigidBodyComp->IsKinematic()) {
            rigidBodyComp->SetPosition(worldPos);
            rigidBodyComp->SetOrientation(worldOrientation);
            // Wake up from sleeping state. Apparently kinematic objects also
            // goes inactive, but are not woken up when we set position.
            rigidBodyComp->GetBulletRigidBody()->activate(true);

            if (rigidBodyComp->GetHaltMovement()) {
                btTransform trans;
                rigidBodyComp->GetBulletRigidBody()->getMotionState()->getWorldTransform(trans);
                // Proceed twice to prevent interpolation of velocities.
                rigidBodyComp->GetBulletRigidBody()->proceedToTransform(trans);
                rigidBodyComp->GetBulletRigidBody()->proceedToTransform(trans);
                rigidBodyComp->SetHaltMovement(false);
            }
        } else if (rigidBodyComp->GetForceTransformSync()) {
            dynamicsWorld->removeRigidBody(rigidBodyComp->GetBulletRigidBody());
            rigidBodyComp->SetPosition(worldPos);
            rigidBodyComp->SetOrientation(worldOrientation);
            rigidBodyComp->GetBulletRigidBody()->activate(true); // To wake up from potentially sleeping state
            dynamicsWorld->addRigidBody(rigidBodyComp->GetBulletRigidBody());
            rigidBodyComp->SetForceTransformSync(false);
        }
    }

    dynamicsWorld->stepSimulation(deltaTime, 10);

    for (auto trigger : triggers) {
        trigger->Process(*dynamicsWorld);
    }
}

void PhysicsManager::UpdateEntityTransforms() {
    for (auto rigidBodyComp : rigidBodyComponents.GetAll()) {
        if (rigidBodyComp->IsKilled() || !rigidBodyComp->entity->IsEnabled())
            continue;

        Entity* entity = rigidBodyComp->entity;
        auto trans = rigidBodyComp->GetBulletRigidBody()->getWorldTransform();
        if (!rigidBodyComp->ghost && !rigidBodyComp->IsKinematic()) {
            entity->SetWorldPosition(Physics::btToGlm(trans.getOrigin()));
            entity->SetWorldOrientation(Physics::btToGlm(trans.getRotation()));
        }
    }
}

void PhysicsManager::OnTriggerEnter(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback) {
    // Add the callback to the trigger observer
    trigger.Open(triggerLockBoxKey, [object, &callback](Physics::Trigger& trigger) { trigger.ForObserver(object->GetBulletCollisionObject(), [&callback](Physics::TriggerObserver& observer) { observer.OnEnter(callback); }); });
}

void PhysicsManager::ForgetTriggerEnter(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object) {
    trigger.Open(triggerLockBoxKey, [object](Physics::Trigger& trigger) { trigger.ForObserver(object->GetBulletCollisionObject(), [](Physics::TriggerObserver& observer) { observer.ForgetEnter(); }); });
}

void PhysicsManager::OnTriggerRetain(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback) {
    // Add the callback to the trigger observer
    trigger.Open(triggerLockBoxKey, [object, &callback](Physics::Trigger& trigger) { trigger.ForObserver(object->GetBulletCollisionObject(), [&callback](::Physics::TriggerObserver& observer) { observer.OnRetain(callback); }); });
}

void PhysicsManager::ForgetTriggerRetain(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object) {
    trigger.Open(triggerLockBoxKey, [object](Physics::Trigger& trigger) { trigger.ForObserver(object->GetBulletCollisionObject(), [](Physics::TriggerObserver& observer) { observer.ForgetRetain(); }); });
}

void PhysicsManager::OnTriggerLeave(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object, std::function<void()> callback) {
    // Add the callback to the trigger observer
    trigger.Open(triggerLockBoxKey, [object, &callback](Physics::Trigger& trigger) { trigger.ForObserver(object->GetBulletCollisionObject(), [&callback](::Physics::TriggerObserver& observer) { observer.OnLeave(callback); }); });
}

void PhysicsManager::ForgetTriggerLeave(Utility::LockBox<Physics::Trigger> trigger, Component::RigidBody* object) {
    trigger.Open(triggerLockBoxKey, [object](Physics::Trigger& trigger) { trigger.ForObserver(object->GetBulletCollisionObject(), [](Physics::TriggerObserver& observer) { observer.ForgetLeave(); }); });
}

Component::RigidBody* PhysicsManager::CreateRigidBody(Entity* owner) {
    auto comp = rigidBodyComponents.Create();
    comp->entity = owner;

    comp->NewBulletRigidBody(1.0f);

    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    if (shapeComp) {
        comp->SetCollisionShape(shapeComp->GetShape());
        comp->SetMass(1.0f);
        dynamicsWorld->addRigidBody(comp->GetBulletRigidBody());
    }

    return comp;
}

Component::Shape* PhysicsManager::CreateShape(Entity* owner) {
    auto comp = shapeComponents.Create();
    comp->entity = owner;

    auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Sphere(1.0f)));
    comp->SetShape(shape);

    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    if (rigidBodyComp) {
        rigidBodyComp->SetCollisionShape(comp->GetShape());
        rigidBodyComp->SetMass(rigidBodyComp->GetMass());
        if (rigidBodyComp->ghost)
            dynamicsWorld->addCollisionObject(rigidBodyComp->GetBulletCollisionObject());
        else
            dynamicsWorld->addRigidBody(rigidBodyComp->GetBulletRigidBody());
    }

    return comp;
}

Utility::LockBox<Physics::Trigger> PhysicsManager::CreateTrigger(std::shared_ptr<Physics::Shape> shape) {
    btTransform trans(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0));
    Physics::Trigger* trigger = new Physics::Trigger(trans);
    trigger->SetCollisionShape(shape);
    triggers.push_back(trigger);
    return Utility::LockBox<Physics::Trigger>(triggerLockBoxKey, trigger);
}

void PhysicsManager::ReleaseTriggerVolume(Utility::LockBox<Physics::Trigger>&& trigger) {
    // If the trigger is the last one standing as was created by us, we find
    // the underlying trigger object and remove it.
    if (trigger.RefCount() == 1) {
        trigger.Open(triggerLockBoxKey, [this](Physics::Trigger& t) {
            auto it = std::find(triggers.begin(), triggers.end(), &t);
            if (it != triggers.end()) {
                delete *it;
                std::swap(*it, *triggers.rbegin());
                triggers.pop_back();
            }
        });
    }
}

void PhysicsManager::SetPosition(Utility::LockBox<Physics::Trigger> trigger, const glm::vec3& position) {
    trigger.Open(triggerLockBoxKey, [&position](Physics::Trigger& trigger) { trigger.SetPosition(Physics::glmToBt(position)); });
}

void PhysicsManager::SetShape(Component::Shape* comp, std::shared_ptr<::Physics::Shape> shape) {
    comp->SetShape(shape);

    auto rigidBodyComp = comp->entity->GetComponent<Component::RigidBody>();
    if (rigidBodyComp)
        rigidBodyComp->SetCollisionShape(comp->GetShape());
}

float PhysicsManager::GetMass(Component::RigidBody* comp) {
    return comp->GetMass();
}

void PhysicsManager::SetShape(Utility::LockBox<Physics::Trigger> trigger, std::shared_ptr<Physics::Shape> shape) {
    trigger.Open(triggerLockBoxKey, [shape](Physics::Trigger& trigger) { trigger.SetCollisionShape(shape); });
}

void PhysicsManager::SetMass(Component::RigidBody* comp, float mass) {
    // Setting mass is only valid with a shape because it also sets inertia.
    auto shapeComp = comp->entity->GetComponent<Component::Shape>();
    if (shapeComp)
        comp->SetMass(mass);
}

void PhysicsManager::SetFriction(Component::RigidBody* comp, float friction) {
    comp->SetFriction(friction);
}

void PhysicsManager::SetRollingFriction(Component::RigidBody* comp, float friction) {
    comp->SetRollingFriction(friction);
}

void PhysicsManager::SetSpinningFriction(Component::RigidBody* comp, float friction) {
    comp->SetSpinningFriction(friction);
}

void PhysicsManager::SetRestitution(Component::RigidBody* comp, float cor) {
    comp->SetRestitution(cor);
}

void PhysicsManager::SetLinearDamping(Component::RigidBody* comp, float damping) {
    comp->SetLinearDamping(damping);
}

void PhysicsManager::SetAngularDamping(Component::RigidBody* comp, float damping) {
    comp->SetAngularDamping(damping);
}

void PhysicsManager::MakeKinematic(Component::RigidBody* comp) {
    bool wasGhost = comp->ghost;

    if (wasGhost)
        dynamicsWorld->removeCollisionObject(comp->GetBulletCollisionObject());

    comp->MakeKinematic();

    if (wasGhost)
        dynamicsWorld->addRigidBody(comp->GetBulletRigidBody());
}

void PhysicsManager::MakeDynamic(Component::RigidBody* comp) {
    bool wasGhost = comp->ghost;
    if (wasGhost)
        dynamicsWorld->removeCollisionObject(comp->GetBulletCollisionObject());

    comp->MakeDynamic();

    if (wasGhost)
        dynamicsWorld->addRigidBody(comp->GetBulletRigidBody());
}

void PhysicsManager::SetGhost(Component::RigidBody* comp, bool ghost) {
    if (ghost && !comp->ghost) {
        dynamicsWorld->removeRigidBody(comp->GetBulletRigidBody());
        comp->SetGhost(ghost);
        dynamicsWorld->addCollisionObject(comp->GetBulletCollisionObject());
    } else if (!ghost && comp->ghost) {
        dynamicsWorld->removeCollisionObject(comp->GetBulletCollisionObject());
        comp->SetGhost(ghost);
        dynamicsWorld->addRigidBody(comp->GetBulletRigidBody());
    }
}

void PhysicsManager::ForceTransformSync(Component::RigidBody* comp) {
    comp->SetForceTransformSync(true);
}

void PhysicsManager::HaltMovement(Component::RigidBody* comp) {
    comp->SetHaltMovement(true);
}

const std::vector<Component::Shape*>& PhysicsManager::GetShapeComponents() const {
    return shapeComponents.GetAll();
}

void PhysicsManager::ClearKilledComponents() {
    rigidBodyComponents.ClearKilled([this](Component::RigidBody* body) {
        if (body->ghost)
            dynamicsWorld->removeCollisionObject(body->GetBulletCollisionObject());
        else
            dynamicsWorld->removeRigidBody(body->GetBulletRigidBody());
    });
    shapeComponents.ClearKilled();
}

btDiscreteDynamicsWorld* PhysicsManager::GetDynamicsWorld() {
    return dynamicsWorld;
}
