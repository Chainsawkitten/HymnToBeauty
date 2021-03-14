#include "GLTFEditor.hpp"

#include <imgui.h>

using namespace GUI;

GLTFEditor::GLTFEditor() {
    
}

void GLTFEditor::Show() {
    if (ImGui::Begin(("GLTF: " + filename + "###" + path + "/" + filename).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        if (ImGui::Button("Import")) {
            gltfImporter.Import(path, filename);
        }
    }
    ImGui::End();
}

void GLTFEditor::SetFile(const std::string& path, const std::string& filename) {
    this->path = path;
    this->filename = filename;
}

bool GLTFEditor::IsVisible() const {
    return visible;
}

void GLTFEditor::SetVisible(bool visible) {
    this->visible = visible;
}
