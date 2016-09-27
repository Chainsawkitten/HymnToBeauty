#pragma once

#include "../Widget.hpp"
#include <vector>

class Entity;
class Font;
class Texture2D;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    class Label;
    class StringEditor;
    class ImageTextButton;
    class ComponentEditor;
    class ModelSelector;
    class TextureSelector;
    class SoundSelector;
    class ComponentAdder;
    
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
            
            Geometry::Rectangle* rectangle;
            glm::vec2 size;
            Font* font;
            
            Entity* entity = nullptr;
            bool visible = false;
            char name[128] = "";
            
            std::vector<ComponentEditor*> editors;
            std::size_t scrollPosition;
            
            Label* nameLabel;
            StringEditor* nameEditor;
            
            Texture2D* addComponentTexture;
            ImageTextButton* addComponentButton;
            
            Texture2D* removeEntityTexture;
            ImageTextButton* removeEntityButton;
            
            ComponentAdder* componentAdder;
    };
}
