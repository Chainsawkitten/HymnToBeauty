#pragma once

#include "Widget.hpp"
#include <functional>

class Texture2D;
class Font;
class Entity;
namespace Geometry {
    class Rectangle;
    class OBJModel;
}

namespace GUI {
    /// Displays all the hymn's resources.
    class ResourceList : public Widget {
        public:
            /// Create new resource list.
            /**
             * @param parent Parent widget.
             */
            ResourceList(Widget* parent);
            
            /// Destructor.
            ~ResourceList() override;
            
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
             * @param size New widget size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Set function to call when an entity has been selected.
            /**
             * @param callback Function to call.
             */
            void SetEntitySelectedCallback(std::function<void(Entity*)> callback);
            
            /// Set function to call when a model has been selected.
            /**
             * @param callback Function to call.
             */
            void SetModelSelectedCallback(std::function<void(Geometry::OBJModel*)> callback);
            
            /// Set function to call when a texture has been selected.
            /**
             * @param callback Function to call.
             */
            void SetTextureSelectedCallback(std::function<void(Texture2D*)> callback);
            
        private:
            Geometry::Rectangle* rectangle;
            Font* font;
            glm::vec2 size;
            
            Texture2D* addTexture;
            
            bool addEntityHover = false;
            Entity* selectedEntity = nullptr;
            bool hasEntitySelectedCallback = false;
            std::function<void(Entity*)> entitySelectedCallback;
            
            bool addModelHover = false;
            Geometry::OBJModel* selectedModel = nullptr;
            bool hasModelSelectedCallback = false;
            std::function<void(Geometry::OBJModel*)> modelSelectedCallback;
            
            bool addTextureHover = false;
            Texture2D* selectedTexture = nullptr;
            bool hasTextureSelectedCallback = false;
            std::function<void(Texture2D*)> textureSelectedCallback;
    };
}
