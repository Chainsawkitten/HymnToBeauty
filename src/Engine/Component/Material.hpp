#pragma once

#include "SuperComponent.hpp"

class Entity;
class Texture2D;

namespace Component {
    /// %Component providing a material to Entity.
    class Material : public SuperComponent {
        public:
            /// Create new material.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Material(Entity* entity);
            
            /// Destructor.
            ~Material();
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
            
            /// Load component from JSON node.
            /**
             * @param node JSON node to load from.
             */
            void Load(const Json::Value& node) override;
            
            /// Set diffuse texture from file.
            /**
             * @param filename Path to the image file.
             */
            void SetDiffuse(const char* filename);
            
            /// Set normal texture from file.
            /**
             * @param filename Path to the image file.
             */
            void SetNormal(const char* filename);
            
            /// Set specular texture from file.
            /**
             * @param filename Path to the image file.
             */
            void SetSpecular(const char* filename);
            
            /// Set glow texture from file.
            /**
             * @param filename Path to the image file.
             */
            void SetGlow(const char* filename);
            
            /// Diffuse texture.
            Texture2D* diffuse;
            
            /// Normal texture.
            Texture2D* normal;
            
            /// Specular texture.
            Texture2D* specular;
            
            /// Glow texture.
            Texture2D* glow;
            
        private:
            void LoadTexture(Texture2D*& texture, const std::string& name);
    };
}
