#include "ScriptEditor.hpp"

#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Editor/Util/EditorSettings.hpp>
#include <imgui.h>

using namespace GUI;

void ScriptEditor::Show() {
    if (ImGui::Begin(("Script: " + script->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(script))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        char nameText[255];
        strcpy(nameText, script->name.c_str());
        if (ImGui::InputText("Name", nameText, 255))
            script->name = nameText;
        
        if (ImGui::Button("Edit Script")) {
            std::string filename = Hymn().GetPath() + FileSystem::DELIMITER + "Scripts" + FileSystem::DELIMITER + script->name + ".as";
            FileSystem::ExecuteProgram(EditorSettings::GetInstance().GetString("Text Editor"), "\"" + filename + "\"");
        }
        
        if (ImGui::Button("Build Script"))
            Managers().scriptManager->BuildScript(script->name);
    }
    ImGui::End();
}

const ScriptFile* ScriptEditor::GetScript() const {
    return script;
}

void ScriptEditor::SetScript(ScriptFile* script) {
    this->script = script;
}

bool ScriptEditor::IsVisible() const {
    return visible;
}

void ScriptEditor::SetVisible(bool visible) {
    this->visible = visible;
}
