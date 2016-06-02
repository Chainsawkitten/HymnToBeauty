#include "FileSystem.hpp"

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
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
    
    bool FileExists(const char* filename) {
#if defined(_WIN32) || defined(WIN32)
        // Windows
        struct _stat buf;
        int result = _stat(filename, &buf);
#else
        // MacOS and Linux
        struct stat buf;
        int result = stat(Name.c_str(), &buf);
#endif
        return result == 0;
    }
    
    void CreateDirectory(const char* filename) {
#if defined(_WIN32) || defined(WIN32)
        // Windows
        _mkdir(filename);
#else
        // MacOS and Linux
        mkdir(filename, ACCESSPERMS);
#endif
    }
    
    std::string DataPath(const char* appName) {
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
        
        CreateDirectory(path.c_str());
        
        return path;
    }
    
    std::string DataPath(const char* appName, const char* filename) {
        std::string path = DataPath(appName);
        path += DELIMITER;
        path += filename;
        
        return path;
    }
}
