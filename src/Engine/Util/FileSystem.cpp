#include "FileSystem.hpp"

#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>

// Platform-dependent includes.
#if defined(_WIN32) || defined(WIN32)
#include <direct.h>
#include <windows.h>
#undef CreateDirectory
#else
#include <dirent.h>
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
    
    std::vector<std::string> DirectoryContents(const std::string& directoryName) {
        std::vector<std::string> contents;
        
#if defined(_WIN32) || defined(WIN32)
        // Windows
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((directoryName + DELIMITER + "*").c_str(), &findFileData);
        bool find = hFind != INVALID_HANDLE_VALUE;
        
        while (find) {
            contents.push_back(findFileData.cFileName);
            find = FindNextFile(hFind, &findFileData);
        }
        
        FindClose(hFind);
#else
        // MacOS and Linux
        DIR* directory = opendir(directoryName.c_str());
        dirent* entry;
        while ((entry = readdir(directory)) != NULL)
            contents.push_back(entry->d_name);
        closedir(directory);
#endif
        
        return contents;
    }
}
