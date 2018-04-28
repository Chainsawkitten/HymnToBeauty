#include "TextureEditor.hpp"

#include <Engine/Texture/TextureAsset.hpp>
#include <Video/Texture/TexturePNG.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;

TextureEditor::TextureEditor() {
    name[0] = '\0';
}

void TextureEditor::Show() {
    if (ImGui::Begin(("Texture: " + texture->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(texture))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        if (ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Rename texture files.
            std::string path = Hymn().GetPath() + "/" + texture->path;
            rename((path + texture->name + ".png").c_str(), (path + name + ".png").c_str());

            texture->name = name;
        }

        if (texture->GetTexture()->IsLoaded()) {
            ImGui::Image((void*) texture->GetTexture()->GetTextureID(), ImVec2(128, 128));
        } else {
            ImGui::Text("Not loaded");
        }

        if (ImGui::Button("Load PNG image")) {
            fileSelector.AddExtensions("png");
            fileSelector.SetInitialPath(Hymn().GetPath().c_str());
            fileSelector.SetFileSelectedCallback(std::bind(&TextureEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }
    }
    ImGui::End();

    if (fileSelector.IsVisible())
        fileSelector.Show();
}

const TextureAsset* TextureEditor::GetTexture() const {
    return texture;
}

void TextureEditor::SetTexture(TextureAsset* texture) {
    this->texture = texture;

    strcpy(name, texture->name.c_str());
}

bool TextureEditor::IsVisible() const {
    return visible;
}

void TextureEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void TextureEditor::FileSelected(const std::string& file) {
    path = file;

    std::string destination = Hymn().GetPath() + "/" + texture->path + texture->name + ".png";
    FileSystem::Copy(file.c_str(), destination.c_str());
    texture->Load(texture->path + texture->name);
}
