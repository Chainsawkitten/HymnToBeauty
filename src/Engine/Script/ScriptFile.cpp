#include "ScriptFile.hpp"

Json::Value ScriptFile::Save() const {
    Json::Value script;
    script["name"] = name;

    return script;
}

void ScriptFile::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);

    pos = this->name.find_last_of('.');
    module = this->name.substr(0, pos);
}
