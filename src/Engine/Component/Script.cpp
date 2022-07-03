#include "Script.hpp"

#include <angelscript.h>
#include <cstring>

#include "../Manager/Managers.hpp"
#include "../Manager/ScriptManager.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Entity/Entity.hpp"
#include "../Script/ScriptFile.hpp"

using namespace Component;

Script::Script() {}

Script::~Script() {
    if (instance != nullptr)
        instance->Release();

    if (scriptFile != nullptr)
        Managers().resourceManager->FreeScriptFile(scriptFile);

    ClearPropertyMap();
}

void Script::Serialize(Json::Value& node, bool load) {
    if (load) {
        Load(node);
    } else {
        node = Save();
    }
}

void Script::AddToPropertyMap(const std::string& name, int type, int size, void* data) {
    if (!IsInPropertyMap(name, type))
        propertyMap[name] = new Property(type, size, data);
}

void Script::CopyDataFromPropertyMap(const std::string& name, void* target) {
    std::memcpy(target, propertyMap[name]->data, propertyMap[name]->size);
}

void* Script::GetDataFromPropertyMap(const std::string& name) {
    return propertyMap[name]->data;
}

bool Script::IsInPropertyMap(const std::string& name, int type) {
    auto it = propertyMap.find(name);
    return it != propertyMap.end() && it->second->typeID == type;
}

void Script::ClearPropertyMap() {
    for (auto pair : propertyMap)
        delete pair.second;
    propertyMap.clear();
}

Json::Value Script::Save() const {
    Json::Value component;
    if (scriptFile != nullptr)
        component["scriptName"] = scriptFile->path + scriptFile->name;

    for (auto& nameProperty : propertyMap) {
        const std::string& name = nameProperty.first;
        int typeId = nameProperty.second->typeID;
        void* varPointer = nameProperty.second->data;
        int size = nameProperty.second->size;

        for (int i = 0; i < size; i++)
            component["propertyMap"][name][std::to_string(typeId)][i] = ((unsigned char*)varPointer)[i];
    }
    return component;
}

void Script::Load(Json::Value& node) {
    // Load values from Json node.
    std::string name = node.get("scriptName", "").asString();
    scriptFile = Managers().resourceManager->CreateScriptFile(name);

    if (node.isMember("propertyMap")) {

        Json::Value propertyMapJson = node.get("propertyMap", "");
        std::vector<std::string> names = propertyMapJson.getMemberNames();

        for (auto& name : names) {
            if (propertyMapJson.isMember(name)) {
                Json::Value typeId_value = propertyMapJson.get(name, "");

                std::vector<std::string> typeIds = typeId_value.getMemberNames();
                int typeId = std::atoi(typeIds[0].c_str());
                int size = typeId_value[typeIds[0]].size();
                void* data = malloc(size + 1);
                for (int i = 0; i < size; i++)
                    ((unsigned char*)data)[i] = (unsigned char)(typeId_value[typeIds[0]][i].asInt());

                AddToPropertyMap(name, typeId, size, data);
                std::free(data);
            }
        }
    }
}
