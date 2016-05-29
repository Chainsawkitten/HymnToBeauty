#pragma once

#include <Engine/Util/Settings.hpp>

/// %Settings for the editor.
/**
 * Available settings:
 * Name          | Description                     | Type | Default Value
 * ------------- | ------------------------------- | ---- | -------------
 * Logging       | Output a log file.              | bool | false
 * Debug Context | Create an OpenGL Debug Context. | bool | false
 */
class EditorSettings : public Settings {
    public:
        /// Get the instance of the class.
        /**
         * @return The %EditorSettings instance
         */
        static EditorSettings& GetInstance();
        
        /// Save the settings to an INI file.
        void Save();
        
    private:
        EditorSettings();
        
        EditorSettings(EditorSettings const&) = delete;
        void operator=(EditorSettings const&) = delete;
};
