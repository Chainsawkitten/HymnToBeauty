#include "ScriptFile.hpp"

#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"

Json::Value ScriptFile::Save() const {
    Json::Value script;
    script["path"] = path;
    script["module"] = module;
    script["name"] = name;

    return script;
}

void ScriptFile::Load(const Json::Value& node) {
    path = node.get("path", "").asString();
    module = node.get("module", "").asString();
    name = node.get("name", "").asString();
}