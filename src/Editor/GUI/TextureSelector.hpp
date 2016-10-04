#pragma once

#include "Container.hpp"
#include <functional>

class Texture2D;
class Font;
namespace Geometry {
    class Rectangle;
    class Model;
}

namespace GUI {
    class ImageButton;
    class VerticalScrollLayout;
    
    /// A window where a model can be selected.
    class TextureSelector : public Container {
        public:
            /// Create new window.
            /**
             * @param parent Parent widget.
             */
            TextureSelector(Widget* parent);
            
            /// Destructor.
            ~TextureSelector() override;
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            void Render() override;
            
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
            
            /// Set function to call when a texture has been selected.
            /**
             * @param callback Function to call.
             */
            void SetTextureSelectedCallback(std::function<void(Texture2D*)> callback);
            
            /// Set the textures to select from.
            /**
             * @param textures The textures to select from.
             */
            void SetTextures(const std::vector<Texture2D*>* textures);
            
            /// Update the list of textures.
            void UpdateTextures();
            
        private:
            void Close();
            void TextureSelected(Texture2D* model);
            
            Geometry::Rectangle* rectangle;
            Font* font;
            
            glm::vec2 size;
            
            bool hasTextureSelectedCallback = false;
            std::function<void(Texture2D*)> textureSelectedCallback;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            const std::vector<Texture2D*>* textures;
            VerticalScrollLayout* textureList;
    };
}
