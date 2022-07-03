#include "Trigger.hpp"
#include "../Trigger/SuperTrigger.hpp"
#include "../Trigger/TriggerRepeat.hpp"
#include "../Physics/Shape.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/PhysicsManager.hpp"

using namespace Component;

Trigger::Trigger() {
    // ADD CONSTRUCTOR
}

Trigger::~Trigger() {
    if (superTrigger != nullptr)
        delete superTrigger;
}

void Trigger::Serialize(Json::Value& node, bool load) {
    if (load) {
        Load(node);
    } else {
        node = Save();
    }
}

SuperTrigger* Trigger::GetTrigger() {
    return superTrigger;
}

void Trigger::SetTrigger(SuperTrigger* trigger) {
    delete superTrigger;
    superTrigger = trigger;
}

Json::Value Trigger::Save() const {
    /// @todo Cleanup
    Json::Value component;

    component = superTrigger->Save();

    return component;
}

void Trigger::Load(Json::Value& node) {
    auto repeat = new TriggerRepeat;
    std::string name = node.get("trigger", "").asString();
    auto triggerVolume = Managers().physicsManager->CreateTrigger(std::make_shared<Physics::Shape>(Physics::Shape::Sphere(1.0f)));
    repeat->triggerVolume = triggerVolume;

    if (!name.empty()) {
        triggerEvent::EventStruct eventstruct;

        repeat->name = node.get("triggerName", "").asString();
        repeat->startActive = node.get("triggerActive", false).asBool();
        repeat->delay = node.get("triggerDelay", 0).asFloat();
        repeat->cooldown = node.get("triggerCooldown", 0).asFloat();
        repeat->triggerCharges = node.get("triggerCharges", 0).asInt();
        repeat->targetFunction.push_back(node.get("triggerFunction", "").asString());

        repeat->collidedEntityUID = node.get("triggerCollidedEntityUID", 0).asInt();
        repeat->targetEntityUID = node.get("triggerTargetEntity", 0).asInt();
        repeat->owningEntityUID = node.get("triggerOwner", 0).asInt();

        eventstruct.m_eventID = node.get("triggerEventStruct_EventID", 0).asInt();
        eventstruct.m_shapeID = node.get("triggerEventStruct_ShapeID", 0).asInt();
        eventstruct.m_targetID = node.get("triggerEventStruct_TargetID", 0).asInt();
        eventstruct.m_scriptID = node.get("triggerEventStruct_ScriptID", 0).asInt();
        eventstruct.check[0] = node.get("triggerEventStruct_Check_0", false).asBool();
        eventstruct.check[1] = node.get("triggerEventStruct_Check_1", false).asBool();
        eventstruct.check[2] = node.get("triggerEventStruct_Check_2", false).asBool();
        eventstruct.check[3] = node.get("triggerEventStruct_Check_3", false).asBool();

        repeat->eventVector.push_back(eventstruct);

        repeat->triggered = node.get("triggerTriggered", false).asBool();

        repeat->triggerVolume = triggerVolume;
    }

    SetTrigger(repeat);
}
