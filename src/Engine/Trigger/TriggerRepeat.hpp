#pragma once

#include <memory>
#include <string>
#include <Utility/LockBox.hpp>
#include <vector>
#include "SuperTrigger.hpp"


class Entity;
class TriggerManager;

namespace triggerEvent {
    struct EventStruct {
        int m_eventID = 0;
        int m_shapeID = 0;
        int m_targetID = 0;
        int m_scriptID = 0;
        bool check[4] = { false }; /// Simple check, should probably be replaced soon.
    };
}

namespace Physics {
    class Trigger;
}

/// %Trigger that can be executed multiple times.
class TriggerRepeat : public SuperTrigger {
    friend class ::TriggerManager;

    public:
        /// Constructor.
        TriggerRepeat();

        /// Destructor.
        ~TriggerRepeat();

        /// Setup the trigger to listen for `enter` events on the trigger
        /// volume, forgetting any previously set listener
        void OnEnter();

        /// Setup the trigger to listen for `retain` events on the trigger
        /// volume, forgetting any previously set listener
        void OnRetain();

        /// Setup the trigger to listen for `leave` events on the trigger
        /// volume, forgetting any previously set listener
        void OnLeave();

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

        /// Vector containing name of target functions.
        /**
         * @return Pointer to the vector containing name of target functions.
         */
        std::vector<std::string>* GetTargetFunction();

        /// Get if the trigger starts active or not.
        /**
         * @return If the trigger starts active or not.
         */
        bool GetStartActive() const;

        /// Set if the trigger starts active or not.
        /**
         * @param value If the trigger starts active or not.
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

        /// Get the cooldown before the trigger can get activated again.
        /**
         * @return The cooldown before the trigger can get activated again.
         */
        float GetCooldown() const;

        /// Set the cooldown before the trigger can get activated again.
        /**
         * @param value The cooldown before the trigger can get activated again.
         */
        void SetCooldown(float value);

        /// Get the amount of times a trigger can be activated before it is no longer active.
        /**
         * @return The amount of times a trigger can be activated before it is no longer active.
         */
        int GetTriggerCharges() const;

        /// Set the amount of times a trigger can be activated before it is no longer active.
        /**
         * @param value The amount of times a trigger can be activated before it is no longer active.
         */
        void SetTriggerCharges(int value);

        /// Vector containing target entities.
        /**
         * @return Pointer to the vector containing target entities.
         */
        std::vector<Entity*>* GetTargetEntity();

        /// Vector containing collided entities.
        /**
         * @return Pointer to the vector containing collided entities.
         */
        std::vector<Entity*>* GetCollidedEntity();

        /// Vector containing the event data.
        /**
         * @return Pointer to the vector containing event data.
         */
        std::vector<triggerEvent::EventStruct>* GetEventVector();

        /// Get the entity this component belongs to.
        /**
         * @return The entity this component belongs to.
         */
        const Entity* GetOwningEntity() const;

        /// Set the entity this component belongs to.
        /**
         * @param value The entity this component belongs to.
         */
        void SetOwningEntity(Entity* value);

        void Process() override;
        void Update() override;
        Json::Value Save() override;
        void InitTriggerUID() override;
        void InitiateVolumes() override;

        /// Set UID for this triggers collided entity.
        /**
         * @param value Set UID for collided entity.
         */
        void SetCollidedEntityUID(int value);

        /// Get UID for this triggers collided entity.
        /**
         * @return int unique identifier for collided entity.
         */
        int GetCollidedEntityUID();

    private:
        void HandleTriggerEvent();

        std::string name = "New Trigger";
        bool startActive = false;
        float delay = 0;
        float cooldown = 0;
        int triggerCharges = 0;
        std::vector<std::string> targetFunction;
        std::vector<Entity*> targetEntity;
        std::vector<Entity*> collidedEntity;
        std::vector<triggerEvent::EventStruct> eventVector;
        Utility::LockBox<Physics::Trigger> triggerVolume;
        bool triggered = false;
        Entity* owningEntity = nullptr;

        int targetEntityUID = 0;
        int collidedEntityUID = 0;
        int owningEntityUID = 0;
};
