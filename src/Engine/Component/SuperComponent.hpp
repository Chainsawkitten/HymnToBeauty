#pragma once

#include <json/json.h>

class Entity;

namespace Component {
/// %Component which all components inherit.
class SuperComponent {
  public:
    /// Create new %SuperComponent.
    SuperComponent();

    /// Destructor.
    virtual ~SuperComponent();

    /// Kill the component, will be removed at the end of the frame.
    void Kill();

    /// Get whether component has been killed.
    /**
     * @return Whether component has been killed.
     */
    bool IsKilled() const;

    /// Save or load component values to/from JSON.
    /**
     * @param node The JSON node to save to or load from.
     * @param load Whether to load (otherwise saves).
     */
    virtual void Serialize(Json::Value& node, bool load) = 0;

    /// Pointer to which entity this component corresponds.
    Entity* entity = nullptr;

  private:
    bool killed = false;
};
} // namespace Component
