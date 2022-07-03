#pragma once

#include <memory>
#include "../Entity/ComponentContainer.hpp"

class SuperTrigger;
class TriggerRepeat;

namespace Component {
class Trigger;
}

namespace Physics {
class Shape;
}

/// Trigger management.
class TriggerManager {
    friend class Hub;

  public:
    /// Iterate triggers and have them call their target scripts if
    /// conditions are satisfied. This should be called after both the
    /// physics- and script managers have been updated.
    void ProcessTriggers();

    /// Create a trigger component.
    /**
     * @return The created component.
     */
    Component::Trigger* CreateTrigger();

    /// Add a repeating trigger to the component.
    /**
     * @param trigger Component to add trigger to.
     * @param shape The physical shape of the trigger volume.
     */
    void AddTriggerRepeat(Component::Trigger* trigger, std::shared_ptr<Physics::Shape> shape);

    /// Get the repeat trigger of a trigger component.
    /**
     * @param trigger Component to access.
     * @return The repeat trigger if present, or nullptr if something else.
     */
    TriggerRepeat* GetTriggerRepeat(const Component::Trigger& trigger) const;

    /// Get all trigger components.
    /**
     * @return All trigger components.
     */
    const std::vector<Component::Trigger*>& GetTriggerComponents() const;

    /// Update the trigger volume data.
    void SynchronizeTriggers();

    /// Find and set entity with Unique ID.
    void InitiateUID();

    /// Set trigger volumes.
    void InitiateVolumes();

    /// Remove all killed components.
    void ClearKilledComponents();

  private:
    TriggerManager();
    ~TriggerManager();
    TriggerManager(const TriggerManager&) = delete;
    void operator=(const TriggerManager&) = delete;

    ComponentContainer<Component::Trigger> triggerComponents;
};
