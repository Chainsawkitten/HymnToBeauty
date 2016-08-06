#pragma once

#include "Container.hpp"
#include <functional>

class Texture2D;
class Font;
namespace Geometry {
    class Rectangle;
    class OBJModel;
}

namespace GUI {
    class ImageButton;
    class VerticalScrollLayout;
    
    /// A window where a model can be selected.
    class ModelSelector : public Container {
        public:
            /// Create new window.
            /**
             * @param parent Parent widget.
             */
            ModelSelector(Widget* parent);
            
            /// Destructor.
            ~ModelSelector() override;
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 GetSize() const override;
            
            /// Set the size of the widget.
            /**
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size);
            
            /// Set function to call when a model has been selected.
            /**
             * @param callback Function to call.
             */
            void SetModelSelectedCallback(std::function<void(Geometry::OBJModel*)> callback);
            
            /// Set the models to select from.
            /**
             * @param models The models to select from.
             */
            void SetModels(const std::vector<Geometry::OBJModel*>* models);
            
            /// Update the list of models.
            void UpdateModels();
            
        private:
            void Close();
            void ModelSelected(Geometry::OBJModel* model);
            
            Geometry::Rectangle* rectangle;
            Font* font;
            
            glm::vec2 size;
            
            bool hasModelSelectedCallback = false;
            std::function<void(Geometry::OBJModel*)> modelSelectedCallback;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            const std::vector<Geometry::OBJModel*>* models;
            VerticalScrollLayout* modelList;
    };
}