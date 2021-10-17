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
        if (ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
            // Rename script file.
            rename((Hymn().GetPath() + "/" + script->path + script->name).c_str(), (Hymn().GetPath() + "/" + script->path + name + ".as").c_str());

            script->name = std::string(name) + ".as";
        }

        if (ImGui::Button("Edit Script")) {
            std::string filename = Hymn().GetPath() + "/" + script->path + script->name;
            FileSystem::ExecuteProgram(EditorSettings::GetInstance().GetString("Text Editor"), "\"" + filename + "\"");
        }

        // Color build button based on build status.
        Status oldStatus = status;
        if (status != UNKNOWN) {
            ImVec4 statusColor(0.5f, 0.75f, 0.5f, 1.0f);
            if (status == FAILURE)
                statusColor = ImVec4(0.8f, 0.4f, 0.4f, 1.0f);

            ImGui::PushStyleColor(ImGuiCol_Button, statusColor);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(statusColor.x * 1.1f, statusColor.y * 1.1f, statusColor.z * 1.1f, 1.f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(statusColor.x * 0.9f, statusColor.y * 0.9f, statusColor.z * 0.9f, 1.f));
        }

        if (ImGui::Button("Build Script")) {
            Managers().scriptManager->RegisterInput();
            int r = Managers().scriptManager->BuildScript(script);
            status = r < 0 ? FAILURE : SUCCESS;
        }

        if (oldStatus != UNKNOWN)
            ImGui::PopStyleColor(3);
    }
    ImGui::End();
}

const ScriptFile* ScriptEditor::GetScript() const {
    return script;
}

void ScriptEditor::SetScript(ScriptFile* script) {
    this->script = script;
    status = UNKNOWN;

    std::size_t pos = script->name.find_last_of('.');
    std::string nameWithoutExtension = script->name.substr(0, pos);

    strcpy(name, nameWithoutExtension.c_str());
}

bool ScriptEditor::IsVisible() const {
    return visible;
}

void ScriptEditor::SetVisible(bool visible) {
    this->visible = visible;
}
