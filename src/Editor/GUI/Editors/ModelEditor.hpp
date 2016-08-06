#pragma once

#include "../Widget.hpp"
#include <string>

class Font;
namespace Geometry {
    class Rectangle;
    class OBJModel;
}

namespace GUI {
    class Label;
    class StringEditor;
    class Button;
    class FileSelector;
    
    /// Used to edit a model.
    class ModelEditor : public Widget {
        public:
            /// Create new model editor.
            /**
             * @param parent Parent widget.
             * @param fileSelector File selector to browse with.
             */
            ModelEditor(Widget* parent, FileSelector* fileSelector);
            
            /// Destructor.
            ~ModelEditor();
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
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
             * @param size New widget size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Set the model to edit.
            /**
             * @param model Model to edit.
             */
            void SetModel(Geometry::OBJModel* model);
            
        private:
            void LoadPressed();
            void FileSelected(const std::string& file);
            
            Geometry::Rectangle* rectangle;
            glm::vec2 size;
            Font* font;
            
            Geometry::OBJModel* model = nullptr;
            
            Label* nameLabel;
            StringEditor* nameEditor;
            
            Button* loadButton;
            FileSelector* fileSelector;
    };
}
