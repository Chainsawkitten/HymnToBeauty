#pragma once

#include "SuperComponent.hpp"

class SuperTrigger;
class TriggerManager;

namespace Json {
class Value;
}

namespace Component {
/// %Component defining a trigger event.
class Trigger : public SuperComponent {
    friend class ::TriggerManager;

  public:
    enum TriggerTypes { ONCE = 0, REPEAT, LOOK_AT, PROXIMITY, NUMBER_OF_TYPES };

    /// Create new trigger.
    Trigger();

    /// Destructor.
    ~Trigger() override;

    Json::Value Save() const override;

    /// Get trigger of this component.
    /**
     * @return The %Trigger
     */
    SuperTrigger* GetTrigger();

    /// Get trigger of this component.
    /**
     * @param trigger The trigger to be set.
     */
    void SetTrigger(SuperTrigger* trigger);

    /// What type of trigger type this component is. Set to ONCE by default.
    TriggerTypes triggerType = TriggerTypes::ONCE;

  private:
    SuperTrigger* superTrigger = nullptr;
};
} // namespace Component
