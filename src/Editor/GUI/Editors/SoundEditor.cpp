#include "SoundEditor.hpp"

#include <imgui.h>

using namespace GUI;

SoundEditor::SoundEditor() {
    
}

void SoundEditor::Show() {
    if (ImGui::Begin(("Sound: " + filename + "###" + path + "/" + filename).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        
    }
    ImGui::End();
}

void SoundEditor::SetFile(const std::string& path, const std::string& filename) {
    this->path = path;
    this->filename = filename;
}

bool SoundEditor::IsVisible() const {
    return visible;
}

void SoundEditor::SetVisible(bool visible) {
    this->visible = visible;
}
