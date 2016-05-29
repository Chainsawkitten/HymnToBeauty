#pragma once

#include <string>

/// Functionality to interact with the file system.
namespace FileSystem {
    /// Delimiter, '\' on Windows, '/' elsewhere.
    extern const char DELIMITER;
    
    /// Check if a file exists.
    /**
     * Works for directories as well.
     * @param filename Filename (either relative or absolute) to check.
     * @return Whether the file exists
     */
    bool FileExists(const char* filename);
    
    /// Create a directory if it does not already exist.
    /**
     * Permission for new directory is 0777 on Unix.
     * @param filename Filename (either absolute or relative) for the directory to create.
     */
    void CreateDirectory(const char* filename);
    
    /// Get save path for application data folder.
    /**
     * The application data folder is created if it does not already exist.
     * On Windows, this is \<User>\\AppData\\Roaming\\\<AppName>.
     * On MacOS, this is ~/Library/Application Support/\<AppName>.
     * On Linux, this is ~/.local/\<AppName>.
     * @param appName The name of the app/game.
     * @return The path
     */
    std::string SavePath(const char* appName);
    
    /// Get save path for application data.
    /**
     * The application data folder is created if it does not already exist.
     * On Windows, this is \<User>\\AppData\\Roaming\\\<AppName>\\\<Filename>.
     * On MacOS, this is ~/Library/Application Support/\<AppName>/\<Filename>.
     * On Linux, this is ~/.local/\<AppName>/\<Filename>.
     * @param appName The name of the app/game.
     * @param filename Filename, eg. "settings.ini".
     * @return The path
     */
    std::string SavePath(const char* appName, const char* filename);
}
