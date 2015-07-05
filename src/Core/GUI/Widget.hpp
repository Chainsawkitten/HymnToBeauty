#ifndef WIDGET_HPP
#define WIDGET_HPP

#include <glm/glm.hpp>

/** @ingroup Core
 * @{
 */

namespace GUI {
    /// A %GUI widget.
    class Widget {
        public:
            /// Create new widget.
            /**
             * @param parent Parent widget.
             */
            Widget(Widget* parent);
            
            /// Destructor.
            virtual ~Widget();
            
            /// Get the widget's parent widget.
            /**
             * @return The parent widget
             */
            Widget* Parent() const;
            
            /// Render the widget.
            /**
             * @param screenWidth Width of the screen in pixels.
             * @param screenHeight Height of the screen in pixels.
             */
            virtual void Render(int screenWidth, int screenHeight) = 0;
            
            /// Get widget's position.
            /**
             * @return The widget's position
             */
            const glm::vec2& Position() const;
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            void SetPosition(const glm::vec2& position);
            
        private:
            Widget* parent;
            
            glm::vec2 position;
    };
}

/** @} */

#endif
