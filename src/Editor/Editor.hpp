#pragma once

#include "GUI/Container.hpp"
#include "GUI/SelectHymnWindow.hpp"
#include "GUI/ResourceList.hpp"
#include <string>

class Texture2D;
class Font;
class Entity;
namespace GUI {
    class FileSelector;
    class EntityEditor;
    class ModelEditor;
    class TextureEditor;
    class SoundEditor;
    class ModelSelector;
    class TextureSelector;
    class SoundSelector;
    class ComponentAdder;
}
namespace Geometry {
    class OBJModel;
}
namespace Audio {
    class SoundBuffer;
}

/// Handles the main editor.
class Editor : public GUI::Container {
    public:
        /// Create new editor.
        Editor();

        /// Destructor.
        ~Editor() override;

        /// Update the editor.
        void Update() override;
        
        /// Render the editor.
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
        void SetSize(const glm::vec2& size) override;
        
        /// Show the editor.
        void Show();
        
        /// Save the hymn being edited.
        void Save() const;
        
    private:
        void Play();
        void NewHymn();
        void NewHymnClosed(const std::string& hymn);
        void OpenHymn();
        void OpenHymnClosed(const std::string& hymn);
        void HideEditors();
        void EntitySelected(Entity* entity);
        void ModelSelected(Geometry::OBJModel* model);
        void TextureSelected(Texture2D* texture);
        void SoundSelected(Audio::SoundBuffer* sound);
        
        GUI::SelectHymnWindow selectHymnWindow;
        GUI::FileSelector* fileSelector;
        
        // Resource list.
        GUI::ResourceList resourceList;
        
        // Editors.
        GUI::EntityEditor* entityEditor;
        GUI::ModelEditor* modelEditor;
        GUI::TextureEditor* textureEditor;
        GUI::SoundEditor* soundEditor;
        GUI::ModelSelector* modelSelector;
        GUI::TextureSelector* textureSelector;
        GUI::SoundSelector* soundSelector;
        GUI::ComponentAdder* componentAdder;
        
        Font* font;
};
