#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>

namespace Animation {
    struct SkeletonBone;
}

namespace Animation {
    /// Skeleton, bindpose of animation.
    class Skeleton {
        public:
            /// Constructor.
            Skeleton();

            /// Save skeleton.
            /**
             * @param name Name of file.
             */
            void Save(const std::string& name);

            /// Load skeleton.
            /**
             * @param name Name of skeleton to load.
             */
            void Load(const std::string& name);

            /// Bones of skeleton.
            std::vector<SkeletonBone*> skeletonBones;

            /// Path to skeleton.
            std::string path;

            /// Name of the resource.
            std::string name;

        private:
            void InitSkeleton();
    };
}
