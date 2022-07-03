#include "RigidBody.hpp"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include "../Physics/GlmConversion.hpp"
#include "../Physics/Shape.hpp"
#include "../Entity/Entity.hpp"
#include "Shape.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/PhysicsManager.hpp"
#include "../Util/Json.hpp"

namespace Component {
RigidBody::~RigidBody() {
    Destroy();
}

void RigidBody::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "mass", mass, 1.0f);
    Json::Serialize(node, load, "friction", friction, 0.5f);
    Json::Serialize(node, load, "rollingFriction", rollingFriction, 0.0f);
    Json::Serialize(node, load, "spinningFriction", spinningFriction, 0.0f);
    Json::Serialize(node, load, "cor", restitution, 0.0f);
    Json::Serialize(node, load, "linearDamping", linearDamping, 0.0f);
    Json::Serialize(node, load, "angularDamping", angularDamping, 0.0f);
    Json::Serialize(node, load, "kinematic", kinematic, false);
    Json::Serialize(node, load, "ghost", ghost, false);

    if (load) {
        // Propagate the data to Bullet.
        NewBulletRigidBody(mass);
        SetFriction(friction);
        SetRollingFriction(rollingFriction);
        SetSpinningFriction(spinningFriction);
        SetRestitution(restitution);
        SetLinearDamping(linearDamping);
        SetAngularDamping(angularDamping);
        if (kinematic)
            MakeKinematic();

        if (ghost)
            SetGhost(ghost);

        auto shapeComp = entity->GetComponent<Component::Shape>();
        if (shapeComp) {
            SetCollisionShape(shapeComp->GetShape());
            SetMass(mass);
        }
    }
}

bool RigidBody::IsKinematic() const {
    return kinematic;
}

bool RigidBody::IsGhost() const {
    return ghost;
}

float RigidBody::GetFriction() const {
    return friction;
}

float RigidBody::GetRollingFriction() const {
    return rollingFriction;
}

float RigidBody::GetSpinningFriction() const {
    return spinningFriction;
}

float RigidBody::GetRestitution() const {
    return restitution;
}

float RigidBody::GetLinearDamping() const {
    return linearDamping;
}

float RigidBody::GetAngularDamping() const {
    return angularDamping;
}

btRigidBody* RigidBody::GetBulletRigidBody() {
    return rigidBody;
}

btCollisionObject* RigidBody::GetBulletCollisionObject() {
    return ghost ? ghostObject : static_cast<btCollisionObject*>(rigidBody);
}

void RigidBody::NewBulletRigidBody(float mass) {
    Destroy();

    this->mass = mass;

    // Motion states inform us of movement caused by physics so that we can
    // deal with those changes as needed.
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));

    // Bullet treats zero mass as infinite, resulting in immovable objects.
    btRigidBody::btRigidBodyConstructionInfo constructionInfo(mass, motionState, nullptr, btVector3(0, 0, 0));
    rigidBody = new btRigidBody(constructionInfo);
    rigidBody->setUserPointer(this);

    ghostObject = new btGhostObject();
    ghostObject->setWorldTransform(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
    ghostObject->setCollisionFlags(ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void RigidBody::Destroy() {
    if (rigidBody) {
        delete rigidBody->getMotionState();
        delete rigidBody;
    }

    delete ghostObject;
}

void RigidBody::SetCollisionShape(std::shared_ptr<Physics::Shape> shape) {
    this->shape = shape;
    rigidBody->setCollisionShape(shape->GetShape());
    ghostObject->setCollisionShape(shape->GetShape());
}

glm::vec3 RigidBody::GetPosition() const {
    btTransform trans;
    if (ghost)
        trans = ghostObject->getWorldTransform();
    else if (IsKinematic())
        rigidBody->getMotionState()->getWorldTransform(trans);
    else
        trans = rigidBody->getWorldTransform();

    return Physics::btToGlm(trans.getOrigin());
}

void RigidBody::SetPosition(const glm::vec3& pos) {
    btTransform trans = ghostObject->getWorldTransform();
    trans.setOrigin(Physics::glmToBt(pos));
    ghostObject->setWorldTransform(trans);

    if (IsKinematic()) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        trans.setOrigin(Physics::glmToBt(pos));
        rigidBody->getMotionState()->setWorldTransform(trans);
    } else {
        btTransform trans = rigidBody->getWorldTransform();
        trans.setOrigin(Physics::glmToBt(pos));
        rigidBody->setWorldTransform(trans);
    }
}

glm::quat RigidBody::GetOrientation() const {
    btTransform trans;
    if (ghost)
        trans = ghostObject->getWorldTransform();
    else if (IsKinematic())
        rigidBody->getMotionState()->getWorldTransform(trans);
    else
        trans = rigidBody->getWorldTransform();

    return Physics::btToGlm(trans.getRotation());
}

void RigidBody::SetOrientation(const glm::quat& rotation) {
    btTransform trans = ghostObject->getWorldTransform();
    trans.setRotation(Physics::glmToBt(rotation));
    ghostObject->setWorldTransform(trans);

    if (IsKinematic()) {
        btTransform trans;
        rigidBody->getMotionState()->getWorldTransform(trans);
        trans.setRotation(Physics::glmToBt(rotation));
        rigidBody->getMotionState()->setWorldTransform(trans);
    } else {
        btTransform trans = rigidBody->getWorldTransform();
        trans.setRotation(Physics::glmToBt(rotation));
        rigidBody->setWorldTransform(trans);
    }
}

float RigidBody::GetMass() {
    return mass;
}

void RigidBody::SetMass(float mass) {
    // Bullet provides a method on the shape that we can use to calculate
    // inertia.
    btVector3 inertia;
    rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
    rigidBody->setMassProps(mass, inertia);
    this->mass = mass;
}

void RigidBody::SetFriction(float friction) {
    rigidBody->setFriction(friction);
    this->friction = friction;
}

void RigidBody::SetRollingFriction(float friction) {
    rigidBody->setRollingFriction(friction);
    this->rollingFriction = friction;
}

void RigidBody::SetSpinningFriction(float friction) {
    rigidBody->setSpinningFriction(friction);
    this->spinningFriction = friction;
}

void RigidBody::SetRestitution(float cor) {
    rigidBody->setRestitution(cor);
    this->restitution = cor;
}

void RigidBody::SetLinearDamping(float damping) {
    rigidBody->setDamping(damping, angularDamping);
    this->linearDamping = damping;
}

void RigidBody::SetAngularDamping(float damping) {
    rigidBody->setDamping(linearDamping, damping);
    this->angularDamping = damping;
}

void RigidBody::MakeKinematic() {
    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    kinematic = true;
    ghost = false;
}

void RigidBody::MakeDynamic() {
    rigidBody->setCollisionFlags(rigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
    kinematic = false;
    ghost = false;
}

void RigidBody::SetGhost(bool ghost) {
    this->ghost = ghost;
}

bool RigidBody::GetForceTransformSync() const {
    return forceTransformSync;
}

void RigidBody::SetForceTransformSync(bool sync) {
    forceTransformSync = sync;
}

bool RigidBody::GetHaltMovement() const {
    return haltMovement;
}

void RigidBody::SetHaltMovement(bool halt) {
    haltMovement = halt;
}
} // namespace Component
