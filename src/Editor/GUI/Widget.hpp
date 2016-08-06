#pragma once

#include <glm/glm.hpp>

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
            Widget* GetParent() const;
            
            /// Update the widget.
            virtual void Update() = 0;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            virtual void Render(const glm::vec2& screenSize) = 0;
            
            /// Get widget's position.
            /**
             * @return The widget's position
             */
            const glm::vec2& GetPosition() const;
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            virtual void SetPosition(const glm::vec2& position);
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            virtual glm::vec2 GetSize() const = 0;
            
            /// Set the size of the widget.
            /**
             * @param size New widget size.
             */
            virtual void SetSize(const glm::vec2& size) = 0;
            
            /// Get whether the widget is visible.
            /**
             * @return Whether the widget should be rendered
             */
            bool IsVisible() const;
            
            /// Set whether the widget is visible.
            /**
             * @param visible Whether the widget should be rendered.
             */
            void SetVisible(bool visible);
            
        private:
            Widget* parent;
            
            glm::vec2 position = glm::vec2(0.f, 0.f);
            bool visible = true;
    };
}
