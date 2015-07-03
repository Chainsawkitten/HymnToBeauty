#include "FileSystem.hpp"

#include <cstdlib>
#if defined(_WIN32) || defined(WIN32)
#include <direct.h>
#endif

namespace FileSystem {
#if defined(_WIN32) || defined(WIN32)
	// Windows
	const char DELIMITER = '\\';
#else
	// MacOS and Linux
	const char DELIMITER = '/';
#endif
    
    void CreateDirectory(const char* filename) {
#if defined(_WIN32) || defined(WIN32)
        // Windows
        _mkdir(filename);
#else
        // MacOS and Linux
        mkdir(filename, ACCESSPERMS);
#endif
    }
    
    std::string SavePath(const char* appName) {
        std::string path;
        
#if defined(_WIN32) || defined(WIN32)
        // Windows
        path = getenv("APPDATA");
        path += DELIMITER;
#elif __APPLE__
        // MacOS
        path = "~/Library/Application Support/";
#elif __linux__
        // Linux
        path = "~/.local/";
#endif
        
        path += appName;
        
        /// @todo: Create directory if it doesn't exist
        CreateDirectory(path.c_str());
        
        return path;
    }
    
    std::string SavePath(const char* appName, const char* filename) {
        std::string path = SavePath(appName);
        path += DELIMITER;
        path += filename;
        
        return path;
    }
}
