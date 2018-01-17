#include "Trigger.hpp"
#include "../Trigger/SuperTrigger.hpp"

using namespace Component;

Trigger::Trigger() {
    // ADD CONSTRUCTOR
}

Trigger::~Trigger() {

    if (superTrigger != nullptr) 
        delete superTrigger;
}

Json::Value Trigger::Save() const {
    Json::Value component;

    component = superTrigger->Save();

    return component;
}

SuperTrigger* Trigger::GetTrigger() {
    return superTrigger;
}

void Trigger::SetTrigger(SuperTrigger* trigger) {
    delete superTrigger;
    superTrigger = trigger;
}
