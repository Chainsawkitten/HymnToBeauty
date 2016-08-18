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
            void Update() override;
            
            /// Get all the container's widgets.
            /**
             * @return All widgets in the container.
             */
            const std::vector<Widget*>& GetWidgets() const;
            
            /// Add a widget to the container.
            /**
             * @param widget Widget to add to the container.
             */
            virtual void AddWidget(Widget* widget);
            
            /// Clear all widgets.
            virtual void ClearWidgets();
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            void SetPosition(const glm::vec2& position) override;
            
        protected:
            /// Update widgets.
            void UpdateWidgets();
            
            /// Render the widgets.
            void RenderWidgets();
            
        private:
            std::vector<Widget*> widgets;
    };
}
