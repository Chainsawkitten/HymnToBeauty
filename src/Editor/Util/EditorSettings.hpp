#pragma once

#include <Engine/Util/Settings.hpp>

/// %Settings for the editor.
/**
 * Available settings:
 * Name                   | Description                      | Type   | Default Value
 * ---------------------- | -------------------------------- | ------ | -------------
 * Logging                | Output a log file.               | bool   | false
 * Debug Context          | Create an OpenGL Debug Context.  | bool   | false
 * Width                  | Width of the editor window.      | long   | 800
 * Height                 | Height of the editor window.     | long   | 600
 * Theme                  | Visual theme to use.             | string | Default
 * Sound Source Icons     | Show sound source icons.         | bool   | true
 * Particle Emitter Icons | Show particle emitter icons.     | bool   | true
 * Light Source Icons     | Show light source icons.         | bool   | true
 * Camera Icons           | Show camera icons.               | bool   | true
 * Physics Volumes        | Show physics volumes.            | bool   | true
 * Lighting               | Light the scene.                 | bool   | true
 * Light Volumes          | Show light culling volumes.      | bool   | false
 * Text Editor            | Path to text editor for scripts. | string |
 * Grid Settings          | Whether to show grid settings.   | bool   | false
 * Grid Size              | Size of the grid.                | long   | 100
 * Grid Line Width        | Width of the lines in the grid.  | long   | 3
 * Grid Snap              | Snap to grid when moving.        | bool   | false
 * Grid Snap Size         | Size to snap to.                 | long   | 100
 */
class EditorSettings : public Settings {
    public:
        /// Get the instance of the class.
        /**
         * @return The %EditorSettings instance
         */
        static EditorSettings& GetInstance();

        void Save() final;

    private:
        EditorSettings();

        EditorSettings(EditorSettings const&) = delete;
        void operator=(EditorSettings const&) = delete;
};
