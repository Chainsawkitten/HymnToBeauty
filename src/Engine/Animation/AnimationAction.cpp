#include "AnimationAction.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"

using namespace Animation;

AnimationAction::AnimationAction() {

}

AnimationAction::~AnimationAction() {
    if (animationClip != nullptr)
        Managers().resourceManager->FreeAnimationClip(animationClip);
}

void AnimationAction::Save(std::ofstream* file) {
    Node::Save(file);
    file->write(reinterpret_cast<char*>(animationClipName), 512);
    file->write(reinterpret_cast<char*>(&playbackModifier), sizeof(uint32_t));
    file->write(reinterpret_cast<char*>(&repeat), sizeof(bool));
}

void AnimationAction::Load(std::ifstream* file) {
    Node::Load(file);
    file->read(reinterpret_cast<char*>(animationClipName), 512);
    file->read(reinterpret_cast<char*>(&playbackModifier), sizeof(uint32_t));
    file->read(reinterpret_cast<char*>(&repeat), sizeof(bool));

    if (animationClip != nullptr)
        Managers().resourceManager->FreeAnimationClip(animationClip);

    animationClip = Managers().resourceManager->CreateAnimationClip(animationClipName);
}
