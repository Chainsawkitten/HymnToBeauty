#include "TriggerOnce.hpp"

TriggerOnce::TriggerOnce() {

}

TriggerOnce::~TriggerOnce() {

}

void TriggerOnce::OnEnter() {

}

void TriggerOnce::OnLeave() {

}

void TriggerOnce::OnRemain() {

}

const std::string& TriggerOnce::GetName() const {
    return name;
}

void TriggerOnce::SetName(const std::string& value) {
    name = value;
}

const std::string& TriggerOnce::GetTargetFunction() const {
    return targetFunction;
}

void TriggerOnce::SetTargetFunction(const std::string& value) {
    targetFunction = value;
}

bool TriggerOnce::GetStartActive() const {
    return startActive;
}

void TriggerOnce::SetStartActive(bool value) {
    startActive = value;
}

float TriggerOnce::GetDelay() const {
    return delay;
}

void TriggerOnce::SetDelay(float value) {
    delay = value;
}

Entity* TriggerOnce::GetTargetEntity() {
    return targetEntity;
}

void TriggerOnce::SetTargetEntity(Entity* value) {
    targetEntity = value;
}

Entity* TriggerOnce::GetCollidedEntity() {
    return collidedEntity;
}

void TriggerOnce::SetCollidedEntity(Entity* value) {
    collidedEntity = value;
}
