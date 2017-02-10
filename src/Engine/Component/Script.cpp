#include "Script.hpp"

#include <string>
#include "../Manager/Managers.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Entity/Entity.hpp"
#include "../Script/ScriptFile.hpp"
#include "../Hymn.hpp"

using namespace Component;

Script::Script(Entity* entity) : SuperComponent(entity) {

}

Json::Value Script::Save() const {
    Json::Value component;
    if (scriptfile != nullptr)
        component["Path"] = scriptfile->path;

    return component;
}

void Script::Load(const Json::Value& node) {

    for (ScriptFile* script_file : Hymn().scripts) {

        std::string path = node.get("Path", "").asString();
        if (script_file->path == path) {

            scriptfile = script_file;

        }

    }

}