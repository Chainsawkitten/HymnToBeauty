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
    
    /// Used to edit an entity.
    class EntityEditor : public Widget {
        public:
            /// Create new entity editor.
            /**
             * @param parent Parent widget.
             * @param modelSelector Model selector to use.
             */
            EntityEditor(Widget* parent, ModelSelector* modelSelector);
            
            /// Destructor.
            ~EntityEditor() override;
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            void SetPosition(const glm::vec2& position) override;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 GetSize() const override;
            
            /// Set the size of the widget.
            /**
             * @param size New widget size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Set the entity to edit.
            /**
             * @param entity The entity to edit.
             */
            void SetEntity(Entity* entity);
            
        private:
            Geometry::Rectangle* rectangle;
            glm::vec2 size;
            Font* font;
            
            Entity* entity = nullptr;
            std::vector<ComponentEditor*> editors;
            
            Label* nameLabel;
            StringEditor* nameEditor;
            
            Texture2D* addComponentTexture;
            ImageTextButton* addComponentButton;
    };
}
