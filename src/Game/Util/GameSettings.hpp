#pragma once

#include <Engine/Util/Settings.hpp>

/// %Settings for the game.
/**
 * Available settings:
 * Name                   | Description                      | Type   | Default Value
 * ---------------------- | -------------------------------- | ------ | -------------
 * 
 */
class GameSettings : public Settings {
  public:
    /// Get the instance of the class.
    /**
     * @return The %GameSettings instance
     */
    static GameSettings& GetInstance();

    void Save() final;

    /// Load the settings from INI file.
    void Load();

  private:
    GameSettings();

    GameSettings(GameSettings const&) = delete;
    void operator=(GameSettings const&) = delete;
};
