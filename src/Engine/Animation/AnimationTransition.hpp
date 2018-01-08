#pragma once

#include <glm/glm.hpp>
#include <fstream>
#include "../Util/Node.hpp"

namespace Animation {
    /// Animation transition node.
    class AnimationTransition : public Node {
        public:
            /// Create new animation transition.
            AnimationTransition();
            
            /// Destructor.
            ~AnimationTransition() override;
            
            /// Save the animation transition node.
            /**
             * @param file File to save to.
             */
            void Save(std::ofstream* file) override;

            /// Load the animation transition node.
            /**
             * @param file File to load from.
             */
            void Load(std::ifstream* file) override;

            /// Is the bool static.
            bool isStatic = true;

            /// Index of the transition bool.
            int32_t transitionBoolIndex = -1;

            /// The transition time.
            float transitionTime = 0.5f;

            /// Process of the transition.
            float transitionProcess = 0.0f;
    };
}
