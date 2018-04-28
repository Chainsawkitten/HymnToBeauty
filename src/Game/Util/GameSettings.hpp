#pragma once

#include <Engine/Util/Settings.hpp>

/// %Settings for the game.
/**
 * Available settings:
 * Name                   | Description                      | Type   | Default Value
 * ---------------------- | -------------------------------- | ------ | -------------
 * Texture Reduction      | Reduction in texture quality.    | long   | 1
 * Shadow Map Size        | Dimensions of the shadow map.    | long   | 1024
 */
class GameSettings : public Settings {
    public:
        /// Get the instance of the class.
        /**
         * @return The %GameSettings instance
         */
        static GameSettings& GetInstance();

        /// Save the settings to an INI file.
        void Save() final;

        /// Load the settings from INI file.
        void Load();

    private:
        GameSettings();

        GameSettings(GameSettings const&) = delete;
        void operator=(GameSettings const&) = delete;
};
