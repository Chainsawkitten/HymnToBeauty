#pragma once

#include "../../Widget.hpp"
#include <string>
#include <vector>

class Entity;
class Font;
class Texture2D;

namespace GUI {
    class Label;
    
    /// Used to edit a component.
    class ComponentEditor : public Widget {
        public:
            /// Create new component editor.
            /**
             * @param parent Parent widget.
             * @param title Title.
             */
            ComponentEditor(Widget* parent, const std::string& title);
            
            /// Destructor.
            ~ComponentEditor() override;
            
            /// Update the editor.
            void Update() override;
            
            /// Render the editor.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            void SetPosition(const glm::vec2& position) override;
            
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
            
            /// Set the entity to edit a component of.
            /**
             * @param entity %Entity to edit.
             */
            virtual void SetEntity(Entity* entity) = 0;
            
        protected:
            /// Add an editor.
            /**
             * @param name Name of the editor to add.
             * @param editor The editor to add.
             */
            void AddEditor(const std::string& name, Widget* editor);
            
        private:
            glm::vec2 size;
            Font* font;
            
            Label* titleLabel;
            
            struct LabeledEditor {
                Label* label;
                Widget* editor;
                
                LabeledEditor(Label* label, Widget* editor);
            };
            std::vector<LabeledEditor> editors;
            
            Texture2D* removeComponentTexture;
            bool removeComponentHover = false;
    };
}
