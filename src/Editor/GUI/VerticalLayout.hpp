#pragma once

#include "Container.hpp"

namespace Geometry {
    class Rectangle;
}

namespace GUI {
    /// Vertical container.
    class VerticalLayout : public Container {
        public:
            /// Create new vertical layout.
            /**
             * @param parent Parent widget.
             */
            VerticalLayout(Widget* parent);
            
            /// Destructor.
            ~VerticalLayout() override;
            
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
            virtual void ClearWidgets();
            
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
            glm::vec2 nextPosition;
    };
}
