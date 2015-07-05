#ifndef WIDGET_HPP
#define WIDGET_HPP

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
            
        private:
            Widget* parent;
    };
}

/** @} */

#endif
