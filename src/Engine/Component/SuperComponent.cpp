#include "SuperComponent.hpp"

using namespace Component;

SuperComponent::SuperComponent() {}

SuperComponent::~SuperComponent() {}

void SuperComponent::Kill() {
    killed = true;
}

bool SuperComponent::IsKilled() const {
    return killed;
}
