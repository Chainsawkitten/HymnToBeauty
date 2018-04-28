#include "AnimationTransition.hpp"

using namespace Animation;

AnimationTransition::AnimationTransition() {
}

AnimationTransition::~AnimationTransition() {
}

void AnimationTransition::Save(std::ofstream* file) {
    Node::Save(file);
    file->write(reinterpret_cast<char*>(&isStatic), sizeof(bool));
    file->write(reinterpret_cast<char*>(&transitionBoolIndex), sizeof(int32_t));
    file->write(reinterpret_cast<char*>(&transitionTime), sizeof(float));
}

void AnimationTransition::Load(std::ifstream* file) {
    Node::Load(file);
    file->read(reinterpret_cast<char*>(&isStatic), sizeof(bool));
    file->read(reinterpret_cast<char*>(&transitionBoolIndex), sizeof(int32_t));
    file->read(reinterpret_cast<char*>(&transitionTime), sizeof(float));
}
