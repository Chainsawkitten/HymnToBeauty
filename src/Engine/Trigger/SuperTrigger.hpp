#pragma once

namespace Json {
    class Value;
}

/// %Super class for triggers to inherit from.
class SuperTrigger {
    public:
        /// Create new %SuperTrigger.
        SuperTrigger();

        /// Destructor.
        virtual ~SuperTrigger();

        /// Process the trigger in case of collision.
        virtual void Process() = 0;

        /// Update position for trigger volume.
        virtual void Update() = 0;

        /// Save the trigger.
        /**
         * @return JSON value to be stored on disk.
         */
        virtual Json::Value Save() = 0;

        /// Initialize entity references for triggers via entity UIDs.
        virtual void InitTriggerUID() = 0;

        /// Initialize volumes for triggers.
        virtual void InitiateVolumes() = 0;
};
