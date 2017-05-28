#include "SoundEditor.hpp"

#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Audio/VorbisFile.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;

void SoundEditor::Show() {
    if (ImGui::Begin(("Sound: " + sound->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(sound))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        ImGui::InputText("Name", name, 128);
        sound->name = name;
        
        if (ImGui::Button("Load Ogg Vorbis")) {
            fileSelector.AddExtensions("ogg");
            fileSelector.SetFileSelectedCallback(std::bind(&SoundEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }
    }
    ImGui::End();
    
    if (fileSelector.IsVisible())
        fileSelector.Show();
}

const Audio::SoundBuffer* SoundEditor::GetSound() const {
    return sound;
}

void SoundEditor::SetSound(Audio::SoundBuffer* sound) {
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
    std::string destination = Hymn().GetPath() + FileSystem::DELIMITER + "Sounds" + FileSystem::DELIMITER + sound->name + ".ogg";
    FileSystem::Copy(file.c_str(), destination.c_str());
    Audio::SoundFile* soundFile = new Audio::VorbisFile(file.c_str());
    sound->Load(soundFile);
    delete soundFile;
}
