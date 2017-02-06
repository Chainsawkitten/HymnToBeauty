#include "ScriptEditor.hpp"

#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Editor/Util/EditorSettings.hpp>
#include <functional>
#include <imgui.h>

#include "../FileSelector.hpp"

using namespace GUI;

void ScriptEditor::Show() {
    if (ImGui::Begin(("Script: " + std::to_string(reinterpret_cast<uintptr_t>(script))).c_str(), &visible)) {

		char name_text[255];
		strcpy(name_text, script->name.c_str());
		if (ImGui::InputText("Name", name_text, 255)) {

			script->name = name_text;

		}

		char module_text[255];
		strcpy(module_text, script->module.c_str());
		if (ImGui::InputText("Module", module_text, 255)) {

			script->module = module_text;

		}

        if (ImGui::Button("Load Script")) {
            fileSelector.AddExtensions("as");
            fileSelector.SetFileSelectedCallback(std::bind(&ScriptEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }

		if (ImGui::Button("Edit Script")) {
			FileSystem::ExecuteProgram(EditorSettings::GetInstance().GetString("Text Editor"), "\"" + script->path + "\"");
		}
        if (ImGui::Button("Build Script")) {
          
            Managers().scriptManager->BuildSpecificScript(script->path.c_str());
        
        }
    }
    ImGui::End();
    
    if (fileSelector.IsVisible())
        fileSelector.Show();
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

void ScriptEditor::FileSelected(const std::string& file) {
	script->path = file.c_str();
}
