#include "Theme.hpp"

#include <Engine/Util/FileSystem.hpp>

namespace ImGui {
    void SaveTheme(const char* name) {
        // Make sure Themes directory exists.
        FileSystem::CreateDirectory((FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes").c_str());
    }
}
