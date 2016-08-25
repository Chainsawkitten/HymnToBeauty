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
    class EntityEditor : public Widget {
        public:
            /// Create new entity editor.
            /**
             * @param parent Parent widget.
             * @param modelSelector Model selector to use.
             * @param textureSelector %Texture selector to use.
             * @param soundSelector Sound selector to use.
             * @param componentAdder %Component adder to use.
             */
            EntityEditor(Widget* parent, ModelSelector* modelSelector, TextureSelector* textureSelector, SoundSelector* soundSelector, ComponentAdder* componentAdder);
            
            /// Destructor.
            ~EntityEditor() override;
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            void Render() override;
            
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
            void AddComponentPressed();
            void RemoveEntityPressed();
            
            Geometry::Rectangle* rectangle;
            glm::vec2 size;
            Font* font;
            
            Entity* entity = nullptr;
            std::vector<ComponentEditor*> editors;
            
            Label* nameLabel;
            StringEditor* nameEditor;
            
            Texture2D* addComponentTexture;
            ImageTextButton* addComponentButton;
            
            Texture2D* removeEntityTexture;
            ImageTextButton* removeEntityButton;
            
            ComponentAdder* componentAdder;
    };
}
