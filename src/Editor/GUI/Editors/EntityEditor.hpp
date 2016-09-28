#pragma once

#include <vector>

class Entity;

namespace GUI {
    /// Used to edit an entity.
    class EntityEditor {
        public:
            /// Create new entity editor.
            EntityEditor();
            
            /// Destructor.
            ~EntityEditor();
            
            /// Show the editor.
            void Show();
            
            /// Set the entity to edit.
            /**
             * @param entity The entity to edit.
             */
            void SetEntity(Entity* entity);
            
            /// Get whether the window is visible.
            /**
             * @return Whether the window is visible.
             */
            bool IsVisible() const;
            
            /// Set whether the window should be visible.
            /**
             * @param visible Whether the window should be visible.
             */
            void SetVisible(bool visible);
            
        private:
            void AddComponentPressed();
            
            Entity* entity = nullptr;
            bool visible = false;
            char name[128] = "";
    };
}
