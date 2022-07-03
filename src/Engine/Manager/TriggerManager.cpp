#include "Managers.hpp"
#include "PhysicsManager.hpp"
#include "TriggerManager.hpp"
#include "ScriptManager.hpp"
#include "../Component/Trigger.hpp"
#include "../Entity/Entity.hpp"
#include "../Physics/Shape.hpp"
#include "../Trigger/SuperTrigger.hpp"
#include "../Trigger/TriggerRepeat.hpp"
#include "../Util/Json.hpp"

TriggerManager::TriggerManager() {}

TriggerManager::~TriggerManager() {
    assert(triggerComponents.GetAll().empty());
}

void TriggerManager::ProcessTriggers() {
    for (auto trigger : triggerComponents.GetAll()) {
        if (trigger->IsKilled() || !trigger->entity->IsEnabled())
            continue;

        trigger->superTrigger->Process();
    }
}

Component::Trigger* TriggerManager::CreateTrigger() {
    auto comp = triggerComponents.Create();

    AddTriggerRepeat(comp, std::make_shared<Physics::Shape>(Physics::Shape::Sphere(1.0f)));

    return comp;
}

void TriggerManager::AddTriggerRepeat(Component::Trigger* trigger, std::shared_ptr<Physics::Shape> shape) {
    auto triggerVolume = Managers().physicsManager->CreateTrigger(shape);

    delete trigger->superTrigger;
    auto repeat = new TriggerRepeat;

    repeat->triggerVolume = triggerVolume;

    trigger->superTrigger = repeat;
    trigger->triggerType = Component::Trigger::REPEAT;
}

TriggerRepeat* TriggerManager::GetTriggerRepeat(const Component::Trigger& trigger) const {
    return dynamic_cast<TriggerRepeat*>(trigger.superTrigger);
}

const std::vector<Component::Trigger*>& TriggerManager::GetTriggerComponents() const {
    return triggerComponents.GetAll();
}

void TriggerManager::SynchronizeTriggers() {
    for (auto trigger : triggerComponents.GetAll()) {
        if (trigger->IsKilled() || !trigger->entity->IsEnabled())
            continue;

        trigger->superTrigger->Update();
    }
}

void TriggerManager::ClearKilledComponents() {
    triggerComponents.ClearKilled();
}

void TriggerManager::InitiateUID() {
    for (auto trigger : triggerComponents.GetAll()) {
        if (trigger->IsKilled() || !trigger->entity->IsEnabled())
            continue;

        trigger->superTrigger->InitTriggerUID();
    }
}

void TriggerManager::InitiateVolumes() {
    for (auto trigger : triggerComponents.GetAll()) {
        if (trigger->IsKilled() || !trigger->entity->IsEnabled())
            continue;

        trigger->superTrigger->InitiateVolumes();
    }
}
