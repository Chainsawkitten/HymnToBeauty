#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;
class Texture2D;
namespace Component {
    class Material;
}

namespace GUI {
    class TextButton;
    class TextureSelector;
    
    /// Used to edit a material component.
    class MaterialEditor : public ComponentEditor {
        public:
            /// Create new material editor.
            /**
             * @param parent Parent widget.
             * @param textureSelector Texture selector to use.
             */
            MaterialEditor(Widget* parent, TextureSelector* textureSelector);
            
            /// Destructor.
            ~MaterialEditor() override;
            
            /// Set the entity to edit the material component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            void SelectDiffusePressed();
            void DiffuseSelected(Texture2D* texture);
            
            void SelectNormalPressed();
            void NormalSelected(Texture2D* texture);
            
            void SelectSpecularPressed();
            void SpecularSelected(Texture2D* texture);
            
            void SelectGlowPressed();
            void GlowSelected(Texture2D* texture);
            
            Font* font;
            
            Component::Material* material = nullptr;
            TextButton* selectDiffuseButton;
            TextButton* selectNormalButton;
            TextButton* selectSpecularButton;
            TextButton* selectGlowButton;
            
            TextureSelector* textureSelector;
    };
}
