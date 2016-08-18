#pragma once

#include "GUI/Container.hpp"
#include <string>

class Texture2D;
class Font;
class Entity;
namespace GUI {
    class SelectHymnWindow;
    class FileSelector;
    class HorizontalLayout;
    class VerticalLayout;
    class Button;
    class ResourceList;
    class EntityEditor;
    class ModelEditor;
    class TextureEditor;
    class ModelSelector;
    class TextureSelector;
    class ComponentAdder;
}
namespace Geometry {
    class OBJModel;
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
        
        /// Save the hymn being edited.
        void Save() const;
        
    private:
        void OpenFileMenu();
        void OpenProjectOptions();
        void Play();
        void NewHymn();
        void NewHymnClosed(const std::string& hymn);
        void OpenHymn();
        void OpenHymnClosed(const std::string& hymn);
        void EntitySelected(Entity* entity);
        void ModelSelected(Geometry::OBJModel* model);
        void TextureSelected(Texture2D* texture);
        
        GUI::SelectHymnWindow* childWindow = nullptr;
        GUI::FileSelector* fileSelector;
        
        // Menu bar.
        GUI::HorizontalLayout* menuBar;
        GUI::Button* fileButton;
        Texture2D* fileTexture;
        
        GUI::Button* optionsButton;
        Texture2D* optionsTexture;
        
        GUI::Button* playButton;
        Texture2D* playTexture;
        
        // File menu
        GUI::VerticalLayout* fileMenu;
        GUI::Button* newHymnButton;
        Texture2D* newHymnTexture;
        
        GUI::Button* openHymnButton;
        Texture2D* openHymnTexture;
        
        // Resource list.
        GUI::ResourceList* resourceList;
        
        // Editors.
        GUI::EntityEditor* entityEditor;
        GUI::ModelEditor* modelEditor;
        GUI::TextureEditor* textureEditor;
        GUI::ModelSelector* modelSelector;
        GUI::TextureSelector* textureSelector;
        GUI::ComponentAdder* componentAdder;
        
        Font* font;
};
