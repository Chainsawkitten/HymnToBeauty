#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include "../Util/Node.hpp"

namespace Animation {
    class AnimationClip;
}

namespace Animation {
    /// Animaiton action node.
    class AnimationAction : public Node {
        public:
            /// Create new animation action.
            AnimationAction();
            
            /// Destructor.
            ~AnimationAction() override;
            
            /// Save the animation action node.
            /**
             * @param file File to save to.
             */
            void Save(std::ofstream* file) override;
            
            /// Load the animation action node.
            /**
             * @param file File to load from.
             */
            void Load(std::ifstream* file) override;
            
            /// Name of animation.
            char animationClipName[512] = {'\0'};
            
            /// Can the playback speed be changed with scripting.
            bool isPlaybackModifierStatic = true;
            
            /// Static playback speed.
            float playbackModifier = 1.f;
            
            /// Dynamic playback modifier.
            int32_t playbackModifierFloatIndex = -1;
            
            /// Shall the animation repeat?
            bool repeat = true;
            
            /// Linked animation clip.
            AnimationClip* animationClip = nullptr;
    };
}
