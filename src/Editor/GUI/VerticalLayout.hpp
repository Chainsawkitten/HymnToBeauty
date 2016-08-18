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
            void Render() override;
            
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
            glm::vec2 nextPosition = glm::vec2(0.f, 0.f);
    };
}
