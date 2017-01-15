#pragma once

#include <Engine/Util/Settings.hpp>

/// %Settings for the editor.
/**
 * Available settings:
 * Name                   | Description                     | Type | Default Value
 * ---------------------- | ------------------------------- | ---- | -------------
 * Logging                | Output a log file.              | bool | false
 * Debug Context          | Create an OpenGL Debug Context. | bool | false
 * Width                  | Width of the editor window.     | long | 800
 * Height                 | Height of the editor window.    | long | 600
 * Sound Source Icons     | Show sound source icons.        | bool | true
 */
class EditorSettings : public Settings {
    public:
        /// Get the instance of the class.
        /**
         * @return The %EditorSettings instance
         */
        static EditorSettings& GetInstance();
        
        /// Save the settings to an INI file.
        void Save() final;
        
    private:
        EditorSettings();
        
        EditorSettings(EditorSettings const&) = delete;
        void operator=(EditorSettings const&) = delete;
};
