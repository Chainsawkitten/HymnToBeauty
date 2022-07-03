#pragma once

#include <json/json.h>
#include <glm/glm.hpp>
#include <string>

class TextureAsset;
namespace Geometry {
class Model;
}
namespace Audio {
class SoundBuffer;
}

namespace Json {

/// Serialize a boolean value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, bool& value, bool defaultValue);

/// Serialize an integer value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, int32_t& value, int32_t defaultValue);

/// Serialize an unsigned integer value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, uint32_t& value, uint32_t defaultValue);

/// Serialize a float value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, float& value, float defaultValue);

/// Serialize a vec2 value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, glm::vec2& value, const glm::vec2& defaultValue);

/// Serialize a vec3 value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, glm::vec3& value, const glm::vec3& defaultValue);

/// Serialize a vec4 value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, glm::vec4& value, const glm::vec4& defaultValue);

/// Serialize a quaternion value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, glm::quat& value, const glm::quat& defaultValue);

/// Serialize a string value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 * @param defaultValue Default value if loading and node does not exist.
 */
void Serialize(Value& node, bool load, const std::string& name, std::string& value, const std::string& defaultValue);

/// Serialize a TextureAsset value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 */
void Serialize(Value& node, bool load, const std::string& name, TextureAsset*& value);

/// Serialize a Model value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 */
void Serialize(Value& node, bool load, const std::string& name, Geometry::Model*& value);

/// Serialize a SoundBuffer value.
/**
 * @param node Json node to save to/load from.
 * @param load Whether to load (otherwise saves).
 * @param name The name to store the value under.
 * @param value The value to save/load.
 */
void Serialize(Value& node, bool load, const std::string& name, Audio::SoundBuffer*& value);

} // namespace Json
