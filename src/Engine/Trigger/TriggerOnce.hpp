#pragma once

#include <string>
#include "SuperTrigger.hpp"

class Entity;

/// %Trigger that can be executed once before going inactive.
class TriggerOnce : public SuperTrigger {
  public:
    /// PLEASE DO NOT USE THIS.
    /// THIS CLASS IS STILL UNDER CONSTRUCTION.
    /// PLEASE DO NOT USE THIS.

    TriggerOnce();
    ~TriggerOnce();

    /// Setup the trigger to listen for `enter` events on the trigger
    /// volume, forgetting any previously set listener
    void OnEnter();

    /// Setup the trigger to listen for `retain` events on the trigger
    /// volume, forgetting any previously set listener
    void OnLeave();

    /// Setup the trigger to listen for `leave` events on the trigger
    /// volume, forgetting any previously set listener
    void OnRemain();

    /// Get the name of the trigger.
    /**
     * @return The name of the trigger.
     */
    const std::string& GetName() const;

    /// Set the name of the trigger.
    /**
     * @param value The name of the trigger.
     */
    void SetName(const std::string& value);

    /// Get name of target function.
    /**
     * @return Name of target function.
     */
    const std::string& GetTargetFunction() const;

    /// Set name of target function.
    /**
     * @param value Name of target function.
     */
    void SetTargetFunction(const std::string& value);

    /// If trigger is starting active or not.
    /**
     * @return If trigger is starting active or not.
     */
    bool GetStartActive() const;

    /// Set if trigger is starting active or not.
    /**
     * @param value If trigger is starting active or not.
     */
    void SetStartActive(bool value);

    /// Get the delay before the trigger gets active.
    /**
     * @return The delay before the trigger gets active.
     */
    float GetDelay() const;

    /// Set the delay before the trigger gets active.
    /**
     * @param value The delay before the trigger gets active.
     */
    void SetDelay(float value);

    /// Pointer to target entity.
    /**
     * @return Pointer to target entity.
     */
    Entity* GetTargetEntity();

    /// Set pointer to target entity.
    /**
     * @param value Pointer to target entity.
     */
    void SetTargetEntity(Entity* value);

    /// Pointer to collided entity.
    /**
     * @return Pointer to collided entity.
     */
    Entity* GetCollidedEntity();

    /// Set pointer to collided entity.
    /**
     * @param value Pointer to collided entity.
     */
    void SetCollidedEntity(Entity* value);

  private:
    std::string name = "DEBUG";
    std::string targetFunction = "DEBUG";
    bool startActive = false;
    float delay = 0;
    Entity* targetEntity = nullptr;
    Entity* collidedEntity = nullptr;
};
