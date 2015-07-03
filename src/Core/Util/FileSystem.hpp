#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>

/** @ingroup Core
 * @{
 */

/// Functionality to interact with the file system.
namespace FileSystem {
    /// Delimiter, '\' on Windows, '/' elsewhere.
    extern const char DELIMITER;
    
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

/** @} */

#endif
