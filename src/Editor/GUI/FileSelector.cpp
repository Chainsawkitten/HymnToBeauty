#include "FileSelector.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>
#include <algorithm>

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
    
    if (ImGui::Begin("Select file", &visible, ImGuiWindowFlags_ShowBorders)) {
        if (ImGui::Selectable("..")) {
            OpenParentDirectory();
        }
        
        for (const std::string& directory : directories) {
            if (ImGui::Selectable(("[DIR] " + directory).c_str())) {
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

void FileSelector::SetExtensions(const std::vector<string>& extensions) {
    this->extensions.clear();

    this->extensions.reserve(extensions.size());
    for (std::size_t i = 0; i < extensions.size(); ++i)
        this->extensions.push_back(extensions[i]);
    pathChanged = true;
}

void FileSelector::AddExtensions(const std::string& extension) {
    this->extensions.push_back(extension);
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
        if (std::find(extensions.begin(), extensions.end(), FileSystem::GetExtension(file)) != extensions.end())
            files.push_back(file);
    }
}
