#include "TriggerRepeat.hpp"

#include "../Component/RigidBody.hpp"
#include "../Component/Shape.hpp"
#include "../Entity/Entity.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/PhysicsManager.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Physics/Shape.hpp"
#include "../Hymn.hpp"


TriggerRepeat::TriggerRepeat() {
}

TriggerRepeat::~TriggerRepeat() {
    /// @todo: Vector support
    if (!collidedEntity.empty() && collidedEntity.front()->GetComponent<Component::RigidBody>()) {
        Component::RigidBody* rigidBodyComp = collidedEntity.front()->GetComponent<Component::RigidBody>();
        Managers().physicsManager->ForgetTriggerEnter(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerRetain(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerLeave(triggerVolume, rigidBodyComp);
    }

    Managers().physicsManager->ReleaseTriggerVolume(std::move(triggerVolume));
}

// ADD SUPPORT FOR VECTOR
void TriggerRepeat::OnEnter() {
    if (!collidedEntity.empty() && collidedEntity.front()->GetComponent<Component::RigidBody>()) {
        Component::RigidBody* rigidBodyComp = collidedEntity.front()->GetComponent<Component::RigidBody>();
        Managers().physicsManager->ForgetTriggerEnter(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerRetain(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerLeave(triggerVolume, rigidBodyComp);
        Managers().physicsManager->OnTriggerEnter(triggerVolume, rigidBodyComp, std::bind(&TriggerRepeat::HandleTriggerEvent, this));
    }
}

// ADD SUPPORT FOR VECTOR
void TriggerRepeat::OnRetain() {
    if (!collidedEntity.empty() && collidedEntity.front()->GetComponent<Component::RigidBody>()) {
        Component::RigidBody* rigidBodyComp = collidedEntity.front()->GetComponent<Component::RigidBody>();
        Managers().physicsManager->ForgetTriggerEnter(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerRetain(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerLeave(triggerVolume, rigidBodyComp);
        Managers().physicsManager->OnTriggerRetain(triggerVolume, rigidBodyComp, std::bind(&TriggerRepeat::HandleTriggerEvent, this));
    }
}

// ADD SUPPORT FOR VECTOR
void TriggerRepeat::OnLeave() {

    if (!collidedEntity.empty() && collidedEntity.front()->GetComponent<Component::RigidBody>()) {
        Component::RigidBody* rigidBodyComp = collidedEntity.front()->GetComponent<Component::RigidBody>();
        Managers().physicsManager->ForgetTriggerEnter(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerRetain(triggerVolume, rigidBodyComp);
        Managers().physicsManager->ForgetTriggerLeave(triggerVolume, rigidBodyComp);
        Managers().physicsManager->OnTriggerLeave(triggerVolume, rigidBodyComp, std::bind(&TriggerRepeat::HandleTriggerEvent, this));
    }
}

const std::string& TriggerRepeat::GetName() const {
    return name;
}

void TriggerRepeat::SetName(const std::string& value) {
    name = value;
}

std::vector<std::string>* TriggerRepeat::GetTargetFunction() {
    return &targetFunction;
}

bool TriggerRepeat::GetStartActive() const {
    return startActive;
}

void TriggerRepeat::SetStartActive(bool value) {
    startActive = value;
}

float TriggerRepeat::GetDelay() const {
    return delay;
}

void TriggerRepeat::SetDelay(float value) {
    delay = value;
}

float TriggerRepeat::GetCooldown() const {
    return cooldown;
}

void TriggerRepeat::SetCooldown(float value) {
    cooldown = value;
}

int TriggerRepeat::GetTriggerCharges() const {
    return triggerCharges;
}

void TriggerRepeat::SetTriggerCharges(int value) {
    triggerCharges = value;
}

std::vector<Entity*>* TriggerRepeat::GetTargetEntity() {
    return &targetEntity;
}

std::vector<Entity*>* TriggerRepeat::GetCollidedEntity() {
    return &collidedEntity;
}

void TriggerRepeat::HandleTriggerEvent() {
    triggered = true;
}

std::vector<triggerEvent::EventStruct>* TriggerRepeat::GetEventVector() {
    return &eventVector;
}

const Entity* TriggerRepeat::GetOwningEntity() const{
    return owningEntity;
}

void TriggerRepeat::SetOwningEntity(Entity* value) {
    owningEntity = value;
}

void TriggerRepeat::InitTriggerUID() {
    if (Hymn().GetEntityByGUID(targetEntityUID) != nullptr)
        targetEntity.push_back(Hymn().GetEntityByGUID(targetEntityUID));

    if (Hymn().GetEntityByGUID(collidedEntityUID) != nullptr)
        collidedEntity.push_back(Hymn().GetEntityByGUID(collidedEntityUID));

    if (Hymn().GetEntityByGUID(owningEntityUID) != nullptr)
        owningEntity = Hymn().GetEntityByGUID(owningEntityUID);
}

void TriggerRepeat::Process() {
    // ADD SUPPORT FOR VECTOR
    if (triggered) {
        Managers().scriptManager->ExecuteScriptMethod(targetEntity.front(), targetFunction.front());
        triggered = false;
    }
}

void TriggerRepeat::Update() {
    if (owningEntity != nullptr) {
        Managers().physicsManager->SetPosition(triggerVolume, owningEntity->GetWorldPosition());

        if (owningEntity->GetComponent<Component::Shape>() != nullptr) {
            Managers().physicsManager->SetShape(triggerVolume, owningEntity->GetComponent<Component::Shape>()->GetShape());
        }
    }
}

void TriggerRepeat::InitiateVolumes() {
    if (GetEventVector()->front().check[0] == true &&
        GetEventVector()->front().check[1] == true &&
        GetEventVector()->front().check[2] == true &&
        GetEventVector()->front().check[3] == true && GetEventVector()->front().m_eventID == 0) {
        OnEnter();
    } else if (GetEventVector()->front().check[0] == true &&
        GetEventVector()->front().check[1] == true &&
        GetEventVector()->front().check[2] == true &&
        GetEventVector()->front().check[3] == true && GetEventVector()->front().m_eventID == 1) {
        OnRetain();
    } else if (GetEventVector()->front().check[0] == true &&
        GetEventVector()->front().check[1] == true &&
        GetEventVector()->front().check[2] == true &&
        GetEventVector()->front().check[3] == true && GetEventVector()->front().m_eventID == 2) {
        OnLeave();
    }
}

Json::Value TriggerRepeat::Save() {
    Json::Value component;

    component["trigger"] = "A Trigger";
    component["triggerName"] = name;
    component["triggerActive"] = startActive;
    component["triggerDelay"] = delay;
    component["triggerCooldown"] = cooldown;
    component["triggerCharges"] = triggerCharges;

    if (!targetFunction.empty())
        component["triggerFunction"] = targetFunction.front(); // ADD SUPPORT FOR VECTOR
    if (!targetEntity.empty())
        component["triggerTargetEntity"] = targetEntity.front()->GetUniqueIdentifier(); // ADD SUPPORT FOR VECTOR

    if (!eventVector.empty()) {
        component["triggerEventStruct_EventID"] = eventVector.front().m_eventID; // ADD SUPPORT FOR VECTOR
        component["triggerEventStruct_ShapeID"] = eventVector.front().m_shapeID; // ADD SUPPORT FOR VECTOR
        component["triggerEventStruct_TargetID"] = eventVector.front().m_targetID; // ADD SUPPORT FOR VECTOR
        component["triggerEventStruct_ScriptID"] = eventVector.front().m_scriptID; // ADD SUPPORT FOR VECTOR
        component["triggerEventStruct_Check_0"] = eventVector.front().check[0]; // ADD SUPPORT FOR VECTOR
        component["triggerEventStruct_Check_1"] = eventVector.front().check[1]; // ADD SUPPORT FOR VECTOR
        component["triggerEventStruct_Check_2"] = eventVector.front().check[2]; // ADD SUPPORT FOR VECTOR
        component["triggerEventStruct_Check_3"] = eventVector.front().check[3]; // ADD SUPPORT FOR VECTOR
    }

    component["triggerCollidedEntityUID"] = collidedEntityUID;
    component["triggerTriggered"] = triggered;

    if (owningEntity != nullptr)
        component["triggerOwner"] = owningEntity->GetUniqueIdentifier();

    return component;
}

void TriggerRepeat::SetCollidedEntityUID(int value) {
    collidedEntityUID = value;
}

int TriggerRepeat::GetCollidedEntityUID() {
    return collidedEntityUID;
}
