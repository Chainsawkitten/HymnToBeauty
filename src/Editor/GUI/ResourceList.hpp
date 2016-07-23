#pragma once

#include "Widget.hpp"
#include <functional>

class Texture2D;
class Font;
class Entity;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    /// Displays all the hymn's resources.
    class ResourceList : public Widget {
        public:
            /// Create new resource list.
            /**
             * @param parent Parent widget.
             */
            ResourceList(Widget* parent);
            
            /// Destructor.
            ~ResourceList() override;
            
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
             * @param size New widget size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Set function to call when an entity has been selected.
            /**
             * @param callback Function to call.
             */
            void SetEntitySelectedCallback(std::function<void(Entity*)> callback);
            
        private:
            Geometry::Rectangle* rectangle;
            Font* font;
            
            Texture2D* addTexture;
            bool addHover;
            
            glm::vec2 size;
            
            Entity* selectedEntity;
            bool hasEntitySelectedCallback;
            std::function<void(Entity*)> entitySelectedCallback;
    };
}
