#include "AnimationControllerEditor.hpp"
#include <Engine/Animation/Animation.hpp>
#include <Engine/Animation/AnimationAction.hpp>
#include <Engine/Animation/AnimationController.hpp>
#include <Engine/Animation/AnimationClip.hpp>
#include <Engine/Animation/AnimationTransition.hpp>
#include <Engine/Animation/Bone.hpp>
#include <Engine/Animation/Skeleton.hpp>
#include <Engine/Animation/SkeletonBone.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/Node.hpp>
#include <Utility/Log.hpp>

using namespace GUI;

AnimationControllerEditor::AnimationControllerEditor() {
}

AnimationControllerEditor::~AnimationControllerEditor() {
    if (this->animationController != nullptr)
        animationController->Save(Hymn().GetPath() + "/" + animationController->path + animationController->name + ".asset");
}

void AnimationControllerEditor::SetAnimationController(Animation::AnimationController* animationController) {
    if (this->animationController != nullptr)
        this->animationController->Save(Hymn().GetPath() + "/" + animationController->path + animationController->name + ".asset");

    this->animationController = animationController;
}

Animation::AnimationController* AnimationControllerEditor::GetAnimationController() {
    return animationController;
}

void AnimationControllerEditor::SetVisible(bool visible) {
    NodeEditor::SetVisible(visible);
    if (this->animationController != nullptr)
        animationController->Save(Hymn().GetPath() + "/" + animationController->path + animationController->name + ".asset");
}

void AnimationControllerEditor::ShowContextMenu() {
    if (animationController == nullptr) {
        Log() << "ERROR: No animation controller choosen.\n";
        return;
    }

    // Create menu item for animation actions.
    if (ImGui::MenuItem("Add animation action")) {
        Animation::AnimationAction* newAction = new Animation::AnimationAction;
        std::string name = "Action #" + std::to_string(animationController->animationNodes.size() + 1);
        unsigned int size = static_cast<unsigned int>(name.size()) < 127 ? static_cast<unsigned int>(name.size()) + 1 : 128;
        memcpy(newAction->name, name.c_str(), size);
        newAction->index = static_cast<uint32_t>(animationController->animationNodes.size());
        animationController->animationNodes.push_back(newAction);
    }

    // Create menu item for animation transitions.
    if (ImGui::MenuItem("Add animation transition")) {
        Animation::AnimationTransition* newTransition = new Animation::AnimationTransition;
        std::string name = "Transition #" + std::to_string(animationController->animationNodes.size() + 1);
        unsigned int size = static_cast<unsigned int>(name.size()) < 127 ? static_cast<unsigned>(name.size()) + 1 : 128;
        memcpy(newTransition->name, name.c_str(), size);
        newTransition->index = static_cast<uint32_t>(animationController->animationNodes.size());
        animationController->animationNodes.push_back(newTransition);
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Add bool")) {
        Animation::AnimationController::BoolItem* newBool = new Animation::AnimationController::BoolItem;
        std::string name = "NewBool #" + std::to_string(animationController->boolMap.size() + 1);
        memcpy(newBool->name, name.c_str(), name.size() + 1);
        animationController->boolMap.push_back(newBool);
    }

    if (ImGui::MenuItem("Add float")) {
        Animation::AnimationController::FloatItem* newFloat = new Animation::AnimationController::FloatItem;
        std::string name = "NewFloat #" + std::to_string(animationController->floatMap.size() + 1);
        memcpy(newFloat->name, name.c_str(), name.size() + 1);
        animationController->floatMap.push_back(newFloat);
    }
}

void AnimationControllerEditor::ShowNode(Node* node) {
    // Dynamic cast to AnimationAction.
    Animation::AnimationAction* action = dynamic_cast<Animation::AnimationAction*>(node);
    if (action) {
        ImGui::Text("Action: %s", node->name);
        ImGui::InputText("Name", node->name, 128);

        if (ImGui::Button("Select animation clip##Clip"))
            ImGui::OpenPopup("Select animation clip##Clip");

        // Playback modifier.
        ImGui::Text("Playback modifier");
        if (ImGui::RadioButton("Static", action->isPlaybackModifierStatic))
            action->isPlaybackModifierStatic = true;

        // Keep the radiobuttons on the same line.
        ImGui::SameLine();

        if (ImGui::RadioButton("Dynamic", !action->isPlaybackModifierStatic))
            action->isPlaybackModifierStatic = false;

        // If static modifier select at static float value.
        if (action->isPlaybackModifierStatic)
            ImGui::DragFloat("", &action->playbackModifier, 0.01f);
        else {
            if (ImGui::Button("Select float##Float"))
                ImGui::OpenPopup("Select float##Float");

            if (ImGui::BeginPopup("Select float##Float")) {
                ImGui::Text("Select float");
                ImGui::Separator();
                for (std::size_t i = 0; i < animationController->floatMap.size(); ++i)
                    if (ImGui::Selectable(animationController->floatMap[i]->name)) {
                        action->playbackModifierFloatIndex = static_cast<uint32_t>(i);
                        break;
                    }

                ImGui::EndPopup();
            }
        }

        // Repeat.
        ImGui::Checkbox("Repeat", &action->repeat);

        // Open popup.
        if (ImGui::BeginPopup("Select animation clip##Clip")) {
            ImGui::Text("Select animation");
            ImGui::Separator();

            if (resourceSelector.Show(ResourceList::Resource::Type::ANIMATION_CLIP)) {
                // If clip is attached to action free it.
                if (action->animationClip != nullptr) {
                    Managers().resourceManager->FreeAnimationClip(action->animationClip);
                    action->animationClip = nullptr;
                }

                std::string path = resourceSelector.GetSelectedResource().GetPath();
                action->animationClip = Managers().resourceManager->CreateAnimationClip(path);
                if (path.size() < 512) {
                    memcpy(action->animationClipName, path.c_str(), path.size() + 1);
                } else {
                    Log() << "Error: " << (int)path.size() << "is to long.\n";
                }
            }

            ImGui::EndPopup();
        }
    } else if (dynamic_cast<Animation::AnimationTransition*>(node) != nullptr) {
        ImGui::Text("Transition: %s", node->name);
        ImGui::InputText("Name", node->name, 128);

        // Dynamic cast to AnimationAction.
        Animation::AnimationTransition* transition = dynamic_cast<Animation::AnimationTransition*>(node);

        ImGui::DragFloat("Time", &transition->transitionTime, 0.001f, 0.f, 1.f);
        ImGui::Checkbox("Is static", &transition->isStatic);

        // If is not static.
        if (!transition->isStatic) {
            if (ImGui::Button("Select bool"))
                ImGui::OpenPopup("Select bool##Bool");

            if (ImGui::BeginPopup("Select bool##Bool")) {
                ImGui::Text("Select bool");
                ImGui::Separator();
                for (std::size_t i = 0; i < animationController->boolMap.size(); ++i) {
                    if (ImGui::Selectable(animationController->boolMap[i]->name)) {
                        transition->transitionBoolIndex = static_cast<uint32_t>(i);
                        break;
                    }
                }

                ImGui::EndPopup();
            }
        }
    }
}

void GUI::AnimationControllerEditor::ShowValues() {
    ImGui::Text("Bools");
    ImGui::Separator();

    for (unsigned int i = 0; i < animationController->boolMap.size(); ++i) {
        Animation::AnimationController::BoolItem* item = animationController->boolMap[i];

        if (boolEditIndex == i) {
            ImGui::BeginChild(item->name, ImVec2(0, 98), true);
            ImGui::Text("Bool: %s", item->name);

            ImGui::InputText("Name", item->name, 128);

            ImGui::Checkbox("Value", &item->value);

            if (ImGui::Button("Remove")) {
                delete item;
                item = nullptr;
                animationController->boolMap.erase(animationController->boolMap.begin() + i);
                ImGui::PopID();
                ImGui::EndChild();
                break;
            }

            ImGui::EndChild();
        } else {
            ImGui::BeginChild(item->name, ImVec2(0, 72), true);
            ImGui::Text("Bool: %s", item->name);

            if (ImGui::Button("Edit")) {
                boolEditIndex = i;
                floatEditIndex = -1;
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove")) {
                delete item;
                item = nullptr;
                animationController->boolMap.erase(animationController->boolMap.begin() + i);
                ImGui::EndChild();
                break;
            }

            ImGui::EndChild();
        }
    }

    ImGui::Separator();
    ImGui::NewLine();
    ImGui::Text("Floats");
    ImGui::Separator();

    for (unsigned int i = 0; i < animationController->floatMap.size(); ++i) {
        Animation::AnimationController::FloatItem* item = animationController->floatMap[i];

        if (floatEditIndex == i) {
            ImGui::BeginChild(item->name, ImVec2(0, 98), true);
            ImGui::Text("Float: %s", item->name);
            ImGui::InputText("Name", item->name, 128, ImGuiInputTextFlags_CharsNoBlank);
            ImGui::DragFloat("Value", &item->value, 0.01f);

            if (ImGui::Button("Remove")) {
                delete item;
                item = nullptr;
                animationController->floatMap.erase(animationController->floatMap.begin() + i);
                ImGui::EndChild();
                break;
            }

            ImGui::EndChild();
        } else {
            ImGui::BeginChild(item->name, ImVec2(0, 72), true);
            ImGui::Text("Bool: %s", item->name);

            if (ImGui::Button("Edit")) {
                floatEditIndex = i;
                boolEditIndex = -1;
            }

            ImGui::SameLine();

            if (ImGui::Button("Remove")) {
                delete item;
                item = nullptr;
                animationController->floatMap.erase(animationController->floatMap.begin() + i);
                ImGui::EndChild();
                break;
            }

            ImGui::EndChild();
        }
    }
}

Node** AnimationControllerEditor::GetNodeArray() {
    return &animationController->animationNodes[0];
}

unsigned int AnimationControllerEditor::GetNumNodes() {
    if (animationController == nullptr) {
        Log() << "ERROR: No animation controller choosen.\n";
        return 0;
    }

    return static_cast<unsigned int>(animationController->animationNodes.size());
}

bool AnimationControllerEditor::CanConnect(Node* output, Node* input) {
    if (typeid(*output) == typeid(Animation::AnimationAction)) {
        if (typeid(*input) == typeid(Animation::AnimationTransition))
            return true;

        return false;
    } else if (typeid(*output) == typeid(Animation::AnimationTransition)) {
        if (typeid(*input) == typeid(Animation::AnimationAction))
            return true;

        return false;
    }

    return false;
}
