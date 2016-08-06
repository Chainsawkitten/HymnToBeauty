#pragma once

#include "../Widget.hpp"
#include <vector>

class Entity;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    class ComponentEditor;
    
    /// Used to edit an entity.
    class EntityEditor : public Widget {
        public:
            /// Create new entity editor.
            /**
             * @param parent Parent widget.
             */
            EntityEditor(Widget* parent);
            
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
            
            Entity* entity = nullptr;
            std::vector<ComponentEditor*> editors;
    };
}
