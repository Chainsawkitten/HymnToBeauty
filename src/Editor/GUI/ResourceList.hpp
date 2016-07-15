#pragma once

#include "Widget.hpp"

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
            
        private:
            Geometry::Rectangle* rectangle;
            
            glm::vec2 size;
    };
}
