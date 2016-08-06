#pragma once

#include "Container.hpp"

namespace Geometry {
    class Rectangle;
}

namespace GUI {
    /// Horizontal container.
    class HorizontalLayout : public Container {
        public:
            /// Create new horizontal layout.
            /**
             * @param parent Parent widget.
             */
            HorizontalLayout(Widget* parent);
            
            /// Destructor.
            ~HorizontalLayout() override;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
            /// Add a widget to the container.
            /**
             * @param widget Widget to add to the container.
             */
            void AddWidget(Widget* widget) override;
            
            /// Clear all widgets.
            void ClearWidgets() override;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 GetSize() const override;
            
            /// Set the size of the widget.
            /**
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size) override;
            
        private:
            Geometry::Rectangle* rectangle;
            
            glm::vec2 size;
            glm::vec2 nextPosition = glm::vec2(0.f, 0.f);
    };
}
