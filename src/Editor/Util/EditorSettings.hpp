#ifndef EDITORSETTINGS_HPP
#define EDITORSETTINGS_HPP

#include <Core/Util/Settings.hpp>

/// %Settings for the editor.
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

#endif
