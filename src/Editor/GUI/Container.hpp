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
            
            /// Get all the container's widgets.
            /**
             * @return All widgets in the container.
             */
            const std::vector<Widget*>& Widgets() const;
            
            /// Add a widget to the container.
            /**
             * @param widget Widget to add to the container.
             */
            virtual void AddWidget(Widget* widget);
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            void SetPosition(const glm::vec2& position) override;
            
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
