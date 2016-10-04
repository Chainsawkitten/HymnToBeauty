#include "FileSelector.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>

using namespace GUI;
using namespace std;

FileSelector::FileSelector() {
    path = FileSystem::DataPath("Hymn to Beauty");
}

void FileSelector::Show() {
    if (pathChanged) {
        ScanDirectory();
        pathChanged = false;
    }
    
    if (ImGui::Begin("Select file", &visible)) {
        if (ImGui::Selectable("..")) {
            OpenParentDirectory();
        }
        
        for (const std::string& directory : directories) {
            if (ImGui::Selectable(directory.c_str())) {
                OpenDirectory(directory);
            }
        }
        
        for (const std::string& file : files) {
            if (ImGui::Selectable(file.c_str())) {
                SelectFile(file);
            }
        }
    }
    ImGui::End();
}

void FileSelector::SetFileSelectedCallback(std::function<void(const std::string&)> callback) {
    fileSelectedCallback = callback;
    hasFileSelectedCallback = true;
}

void FileSelector::SetExtension(const string& extension) {
    this->extension = extension;
    pathChanged = true;
}

bool FileSelector::IsVisible() const {
    return visible;
}

void FileSelector::SetVisible(bool visible) {
    this->visible = visible;
}

void FileSelector::OpenParentDirectory() {
    path = FileSystem::GetParentDirectory(path);
    pathChanged = true;
}

void FileSelector::OpenDirectory(const string& name) {
    path += FileSystem::DELIMITER + name;
    pathChanged = true;
}

void FileSelector::SelectFile(const string& name) {
    visible = false;
    
    if (hasFileSelectedCallback)
        fileSelectedCallback(path + FileSystem::DELIMITER + name);
}

void FileSelector::ScanDirectory() {
    directories = FileSystem::DirectoryContents(path, FileSystem::DIRECTORY);
    
    files.clear();
    std::vector<std::string> tempFiles = FileSystem::DirectoryContents(path, FileSystem::FILE);
    for (const std::string& file : tempFiles) {
        if (FileSystem::GetExtension(file) == extension)
            files.push_back(file);
    }
}
