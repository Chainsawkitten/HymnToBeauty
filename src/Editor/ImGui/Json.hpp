#pragma once

#include <json/json.h>
#include <imgui.h>

namespace Json {
/// Convert an ImVec2 to a JSON node.
/**
 * @param value %Value to convert.
 * @return JSON node.
 */
Json::Value SaveImVec2(const ImVec2& value);

/// Load an ImVec2 from a JSON node.
/**
 * @param value The JSON node to load from.
 * @return The loaded ImVec2.
 */
ImVec2 LoadImVec2(const Value& value);

/// Convert an ImVec4 to a JSON node.
/**
 * @param value %Value to convert.
 * @return JSON node.
 */
Json::Value SaveImVec4(const ImVec4& value);

/// Load an ImVec4 from a JSON node.
/**
 * @param value The JSON node to load from.
 * @return The loaded ImVec4.
 */
ImVec4 LoadImVec4(const Value& value);
} // namespace Json
