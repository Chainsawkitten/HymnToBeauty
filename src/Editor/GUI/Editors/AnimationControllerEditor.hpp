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

            /// Set whether the window should be visible.
            /**
             * @param visible Whether the window should be visible.
             */
            void SetVisible(bool visible) override;

        protected:
            /// Show the context menu.
            void ShowContextMenu() override;

            /// Shows the animation node.
            /**
             * @param node Node to be shown.
             */
            void ShowNode(Node* node) override;

            /// Show global values exposed to scripting.
            void ShowValues() override;

            /// Return a node array of animation related nodes.
            /**
             * @return The node array.
             */
            Node** GetNodeArray() override;

            /// Return the number of animation nodes in the array.
            /**
             * @return The number of animation nodes.
             */
            unsigned int GetNumNodes() override;

            /// Returns true if the two nodes can connect.
            /**
             * @param output The output node.
             * @param input The input node.
             * @return True if output and input can connect.
             */
            bool CanConnect(Node* output, Node* input) override;

        private:
            Animation::AnimationController* animationController = nullptr;

            ResourceSelector resourceSelector;

            int boolEditIndex = -1;
            int floatEditIndex = -1;
    };
}
