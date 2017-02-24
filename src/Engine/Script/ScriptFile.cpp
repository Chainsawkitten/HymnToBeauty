#include "ScriptFile.hpp"

Json::Value ScriptFile::Save() const {
    Json::Value script;
    script["name"] = name;
    
    return script;
}

void ScriptFile::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
}
