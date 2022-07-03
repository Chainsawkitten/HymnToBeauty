#include "Json.hpp"

#include <glm/gtc/quaternion.hpp>
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Geometry/Model.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Audio/SoundFile.hpp"

namespace Json {

void Serialize(Value& node, bool load, const std::string& name, bool& value, bool defaultValue) {
    if (load) {
        value = node.get(name, defaultValue).asBool();
    } else {
        node[name] = value;
    }
}

void Serialize(Value& node, bool load, const std::string& name, int32_t& value, int32_t defaultValue) {
    if (load) {
        value = node.get(name, defaultValue).asInt();
    } else {
        node[name] = value;
    }
}

void Serialize(Value& node, bool load, const std::string& name, uint32_t& value, uint32_t defaultValue) {
    if (load) {
        value = node.get(name, defaultValue).asUInt();
    } else {
        node[name] = value;
    }
}

void Serialize(Value& node, bool load, const std::string& name, float& value, float defaultValue) {
    if (load) {
        value = node.get(name, defaultValue).asFloat();
    } else {
        node[name] = value;
    }
}

void Serialize(Value& node, bool load, const std::string& name, glm::vec2& value, const glm::vec2& defaultValue) {
    Value& vecNode = node[name];

    Serialize(vecNode, load, "x", value.x, defaultValue.x);
    Serialize(vecNode, load, "y", value.y, defaultValue.y);
}

void Serialize(Value& node, bool load, const std::string& name, glm::vec3& value, const glm::vec3& defaultValue) {
    Value& vecNode = node[name];

    Serialize(vecNode, load, "x", value.x, defaultValue.x);
    Serialize(vecNode, load, "y", value.y, defaultValue.y);
    Serialize(vecNode, load, "z", value.z, defaultValue.z);
}

void Serialize(Value& node, bool load, const std::string& name, glm::vec4& value, const glm::vec4& defaultValue) {
    Value& vecNode = node[name];

    Serialize(vecNode, load, "x", value.x, defaultValue.x);
    Serialize(vecNode, load, "y", value.y, defaultValue.y);
    Serialize(vecNode, load, "z", value.z, defaultValue.z);
    Serialize(vecNode, load, "w", value.w, defaultValue.w);
}

void Serialize(Value& node, bool load, const std::string& name, glm::quat& value, const glm::quat& defaultValue) {
    Value& vecNode = node[name];

    Serialize(vecNode, load, "x", value.x, defaultValue.x);
    Serialize(vecNode, load, "y", value.y, defaultValue.y);
    Serialize(vecNode, load, "z", value.z, defaultValue.z);
    Serialize(vecNode, load, "w", value.w, defaultValue.w);
}

void Serialize(Value& node, bool load, const std::string& name, std::string& value, const std::string& defaultValue) {
    if (load) {
        value = node.get(name, defaultValue).asString();
    } else {
        node[name] = value;
    }
}

void Serialize(Value& node, bool load, const std::string& name, TextureAsset*& value) {
    if (load) {
        std::string textureName = node.get(name, "").asString();

        if (!textureName.empty())
            value = Managers().resourceManager->CreateTextureAsset(textureName);
    } else if (value != nullptr) {
        node[name] = value->path + value->name;
    }
}

void Serialize(Value& node, bool load, const std::string& name, Geometry::Model*& value) {
    if (load) {
        std::string modelName = node.get(name, "").asString();

        if (!modelName.empty())
            value = Managers().resourceManager->CreateModel(modelName);
    } else if (value != nullptr) {
        node[name] = value->path + value->name;
    }
}

void Serialize(Value& node, bool load, const std::string& name, Audio::SoundBuffer*& value) {
    if (load) {
        std::string soundFileName = node.get(name, "").asString();

        if (!soundFileName.empty())
            value->SetSoundFile(Managers().resourceManager->CreateSound(soundFileName));
    } else if (value->GetSoundFile() != nullptr) {
        node[name] = value->GetSoundFile()->path + value->GetSoundFile()->name;
    }
}
} // namespace Json
