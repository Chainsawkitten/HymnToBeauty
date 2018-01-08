#include "SoundEditor.hpp"

#include <Engine/Audio/SoundFile.hpp>
#include <Engine/Audio/VorbisFile.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;

SoundEditor::SoundEditor() {
    name[0] = '\0';
}

void SoundEditor::Show() {
    if (ImGui::Begin(("Sound: " + sound->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(sound))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        if (ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Rename sound file.
            std::string path = Hymn().GetPath() + "/" + sound->path;
            rename((path + sound->name + ".ogg").c_str(), (path + name + ".ogg").c_str());
            rename((path + sound->name + ".json").c_str(), (path + name + ".json").c_str());

            sound->name = name;
        }
        
        if (ImGui::Button("Load Ogg Vorbis")) {
            fileSelector.AddExtensions("ogg");
            fileSelector.SetInitialPath(Hymn().GetPath().c_str());
            fileSelector.SetFileSelectedCallback(std::bind(&SoundEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }
    }
    ImGui::End();
    
    if (fileSelector.IsVisible())
        fileSelector.Show();
}

const Audio::SoundFile* SoundEditor::GetSound() const {
    return sound;
}

void SoundEditor::SetSound(Audio::SoundFile* sound) {
    this->sound = sound;
    
    strcpy(name, sound->name.c_str());
}

bool SoundEditor::IsVisible() const {
    return visible;
}

void SoundEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void SoundEditor::FileSelected(const std::string& file) {
    std::string destination = Hymn().GetPath() + "/" + sound->path + sound->name + ".ogg";
    FileSystem::Copy(file.c_str(), destination.c_str());
}
