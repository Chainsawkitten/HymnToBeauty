#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include "../Util/Node.hpp"
#include <map>
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include <Utility/Log.hpp>

namespace Animation {
    class AnimationClip;
    class Skeleton;
}

namespace Animation {
    /// An animation loaded from a file.
    class AnimationController {
        public:
            /// Used to map bools.
            struct BoolItem {
                char name[128] = "NewBool\0";
                bool value = false;
            };

            /// Used to map floats.
            struct FloatItem {
                char name[128] = "NewFloat\0";
                float value = 1.f;
            };

            /// Default constructor.
            AnimationController() = default;

            /// Destructor.
            ~AnimationController();

            /// Save animation controller.
            /**
             * @param path Save location.
             */
            void Save(const std::string& path);

            /// Load animation controller.
            /**
             * @param name Load from name.
             */
            void Load(const std::string& name);

            /// Vector with the animation nodes.
            std::vector<Node*> animationNodes;

            /// Used to map bools for animation system.
            std::vector<BoolItem*> boolMap;

            /// Used to map floats for animation system.
            std::vector<FloatItem*> floatMap;

            /// Skeleton.
            Skeleton* skeleton;

            /// Name of resource.
            std::string name;

            /// The folder containing the model file.
            std::string path;

        private:
            enum NodeType {
                ACTION = 0,
                TRANSITION = 1
            };

            void Clear();
    };
}
