#pragma once

#include "NodeEditor.hpp"
#include "../ResourceSelector.hpp"
#include <Engine/Util/Node.hpp>

namespace Animation {
    class AnimationController;
}

namespace GUI {
    /// Animation controller, node editor class.
    class AnimationControllerEditor : public NodeEditor {
        public:
            /// Create new animation controller editor.
            AnimationControllerEditor();

            /// Destructor.
            ~AnimationControllerEditor();

            /// Set the animation controller you wish to edit.
            /**
             * @param animationController Animation controller the user wishes to edit.
             */
            void SetAnimationController(Animation::AnimationController* animationController);

            /// Returns the active animation controller.
            /**
             * @return The active animation controller.
             */
            Animation::AnimationController* GetAnimationController();

            void SetVisible(bool visible) override;

        protected:
            void ShowContextMenu() override;
            void ShowNode(Node* node) override;
            void ShowValues() override;
            Node** GetNodeArray() override;
            unsigned int GetNumNodes() override;
            bool CanConnect(Node* output, Node* input) override;

        private:
            Animation::AnimationController* animationController = nullptr;

            ResourceSelector resourceSelector;

            int boolEditIndex = -1;
            int floatEditIndex = -1;
    };
}
