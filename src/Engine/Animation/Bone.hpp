#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Animation {
    /// Bone data.
    struct Bone {
        /// Destructor.
        ~Bone();

        /// Save bone data.
        /**
         * @param file File to save to.
         */
        void Save(std::ofstream* file);

        /// Load bone data.
        /**
         * @param file File to load from.
         */
        void Load(std::ifstream* file);

        /// Parent id of bone.
        uint32_t parent = 0;

        /// The number of rotation keys.
        uint32_t numRotationKeys = 0;

        /// Keyframed rotations.
        int32_t* rotationKeys = nullptr;

        /// Keyframe rotation values.
        glm::quat* rotations = nullptr;

        /// The current animation index.
        uint32_t currentKeyIndex = 0;
    };
}
