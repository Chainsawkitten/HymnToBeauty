#ifndef CONTAINER_HPP
#define CONTAINER_HPP

#include "Widget.hpp"
#include <vector>

/** @ingroup Core
 * @{
 */

namespace GUI {
    /// A container that contains Widgets.
    class Container : public Widget {
        public:
            /// Create a new container.
            /**
             * @param parent Parent widget.
             */
            Container(Widget* parent);
            
            /// Destructor.
            virtual ~Container();
            
            /// Update the widget.
            void Update();
            
            /// Add a widget to the container.
            /**
             * @param widget Widget to add to the container.
             */
            virtual void AddWidget(Widget* widget);
            
        protected:
            /// Update widgets.
            void UpdateWidgets();
            
            /// Render the widgets.
            /**
             * @param screenWidth Width of the screen in pixels.
             * @param screenHeight Height of the screen in pixels.
             */
            void RenderWidgets(int screenWidth, int screenHeight);
            
        private:
            std::vector<Widget*> widgets;
    };
}

/** @} */

#endif
