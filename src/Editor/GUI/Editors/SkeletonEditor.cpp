#include "SkeletonEditor.hpp"

#include <Engine/Animation/Skeleton.hpp>
#include <Engine/Hymn.hpp>
#include <imgui.h>
#include <Engine/Util/FileSystem.hpp>
#include "../../Util/AssetConverterSkeleton.hpp"

using namespace GUI;

SkeletonEditor::SkeletonEditor() {
    name[0] = '\0';
}

void SkeletonEditor::Show() {
    if (ImGui::Begin(("Skeleton: " + skeleton->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(skeleton))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        if (ImGui::Button("Open skeleton model")) {
            // Currently only fbx is tested.
            fileSelector.AddExtensions("fbx");
            fileSelector.SetInitialPath(Hymn().GetPath().c_str());
            fileSelector.SetFileSelectedCallback(std::bind(&SkeletonEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }
    }
    ImGui::End();

    if (fileSelector.IsVisible())
        fileSelector.Show();
}

Animation::Skeleton* GUI::SkeletonEditor::GetSkeleton() {
    return skeleton;
}

void SkeletonEditor::SetSkeleton(Animation::Skeleton* skeleton) {
    this->skeleton = skeleton;
}

bool SkeletonEditor::IsVisible() const {
    return visible;
}

void SkeletonEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void SkeletonEditor::FileSelected(const std::string& file) {
    std::string name = FileSystem::GetName(file);
    std::string newFile = Hymn().GetPath() + "/" + skeleton->path + name + ".asset";

    AssetConverterSkeleton con;
    con.Convert(file.c_str(), newFile.c_str(), true);
    
    skeleton->name = name;
}
