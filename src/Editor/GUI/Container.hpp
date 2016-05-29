#pragma once

#include "Widget.hpp"
#include <vector>

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
            virtual void Update();
            
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
             * @param screenSize Size of the screen in pixels.
             */
            void RenderWidgets(const glm::vec2& screenSize);
            
        private:
            std::vector<Widget*> widgets;
    };
}
