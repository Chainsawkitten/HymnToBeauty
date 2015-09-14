#ifndef VERTICALLAYOUT_HPP
#define VERTICALLAYOUT_HPP

#include <Core/Geometry/Rectangle.hpp>
#include "Container.hpp"

/** @ingroup Core
 * @{
 */

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
            ~VerticalLayout();
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize);
            
            /// Add a widget to the container.
            /**
             * @param widget Widget to add to the container.
             */
            void AddWidget(Widget* widget);
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 Size() const;
            
            /// Set the size of the widget.
            /**
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size);
            
        private:
            Geometry::Rectangle* rectangle;
            
            glm::vec2 size;
            glm::vec2 nextPosition;
    };
}

/** @} */

#endif
