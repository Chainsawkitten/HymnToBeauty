#pragma once

namespace ImGui {
    /// Save current theme to a JSON file.
    /**
     * @param name The name of the theme.
     */
    void SaveTheme(const char* name);
    
    /// Load theme from JSON file.
    /**
     * @param name The name of the theme.
     */
    void LoadTheme(const char* name);
    
    /// Load the default theme.
    void LoadDefaultTheme();
}
