#pragma once

#include <Engine/Geometry/Rectangle.hpp>
#include "Container.hpp"

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
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 Size() const override;
            
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
