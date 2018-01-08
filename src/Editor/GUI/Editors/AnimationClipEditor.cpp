#include "AnimationClipEditor.hpp"
#include <Engine/Animation/AnimationClip.hpp>
#include <Engine/Hymn.hpp>
#include <imgui.h>
#include <Engine/Util/FileSystem.hpp>
#include "../../Util/AssetConverterSkeleton.hpp"

using namespace GUI;

AnimationClipEditor::AnimationClipEditor() {
}

void AnimationClipEditor::Show() {
    if (ImGui::Begin(("Animation clip: " + animationClip->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(animationClip))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        if (ImGui::Button("Open animation model")) {
            // Currently only fbx is tested.
            fileSelector.AddExtensions("fbx");
        
            // Set the initial path to the models directory.
            fileSelector.SetInitialPath((Hymn().GetPath()).c_str());
            fileSelector.SetFileSelectedCallback(std::bind(&AnimationClipEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }
    }
    ImGui::End();

    if (fileSelector.IsVisible())
        fileSelector.Show();
}

Animation::AnimationClip* GUI::AnimationClipEditor::GetAnimationClip() {
    return animationClip;
}

void AnimationClipEditor::SetAnimationClip(Animation::AnimationClip* animationClip) {
    this->animationClip = animationClip;
}

bool AnimationClipEditor::IsVisible() const {
    return visible;
}

void AnimationClipEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void GUI::AnimationClipEditor::FileSelected(const std::string& file) {
    std::string name = FileSystem::GetName(file);
    std::string newFile = Hymn().GetPath() + "/" + animationClip->path + name + ".asset";

    AssetConverterSkeleton con;
    con.Convert(file.c_str(), newFile.c_str(), false);

    animationClip->name = name;
}
