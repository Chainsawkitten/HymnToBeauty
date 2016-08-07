#pragma once

#include "../../Container.hpp"
#include <Engine/Entity/Entity.hpp>
#include <functional>

class Texture2D;
class Font;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    class ImageButton;
    class VerticalScrollLayout;
    
    /// A window where components can be added to an entity.
    class ComponentAdder : public Container {
        public:
            /// Create new window.
            /**
             * @param parent Parent widget.
             */
            ComponentAdder(Widget* parent);
            
            /// Destructor.
            ~ComponentAdder() override;
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 GetSize() const override;
            
            /// Set the size of the widget.
            /**
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size);
            
            /// Set the entity to add components to.
            /**
             * @param entity The entity to add components to.
             */
            void SetEntity(Entity* entity);
            
        private:
            void Close();
            template<typename T> void ComponentPressed();
            
            Geometry::Rectangle* rectangle;
            Font* font;
            
            glm::vec2 size;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            Entity* entity = nullptr;
            VerticalScrollLayout* componentList;
    };
}

template<typename T> void GUI::ComponentAdder::ComponentPressed() {
    entity->AddComponent<T>();
    SetVisible(false);
}
