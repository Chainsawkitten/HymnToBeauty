#include "FileSystem.hpp"

#include <cstdlib>
#include <sys/stat.h>
#include <cctype>
#include <cstring>
#include <fstream>
#include <Utility/Log.hpp>
#include <stdio.h>

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

    const unsigned int FILE = 1;
    const unsigned int DIRECTORY = 2;

    bool FileExists(const char* filename) {
#if defined(_WIN32) || defined(WIN32)
        // Windows
        struct _stat buf;
        int result = _stat(filename, &buf);
#else
        // MacOS and Linux
        struct stat buf;
        int result = stat(filename, &buf);
#endif
        return result == 0;
    }

    void Copy(const char* source, const char* destination) {
        std::ifstream sourceFile(source, std::ios::binary);
        std::ofstream destinationFile(destination, std::ios::binary);

        destinationFile << sourceFile.rdbuf();

        sourceFile.close();
        destinationFile.close();
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

    std::vector<std::string> DirectoryContents(const std::string& directoryName, unsigned int type) {
        std::vector<std::string> contents;

#if defined(_WIN32) || defined(WIN32)
        // Windows
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile((directoryName + DELIMITER + "*").c_str(), &findFileData);
        bool find = hFind != INVALID_HANDLE_VALUE;

        while (find) {
            if (((type & DIRECTORY && findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ||
                 (type & FILE && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))) &&
                    strcmp(findFileData.cFileName, ".") &&
                    strcmp(findFileData.cFileName, "..")) {
                contents.push_back(findFileData.cFileName);
            }

            find = FindNextFile(hFind, &findFileData);
        }

        FindClose(hFind);
#else
        // MacOS and Linux
        DIR* directory = opendir(directoryName.c_str());
        dirent* entry;
        while ((entry = readdir(directory)) != NULL) {
            if (((type & DIRECTORY && entry->d_type == DT_DIR) ||
                 (type & FILE && entry->d_type != DT_DIR)) &&
                    strcmp(entry->d_name, ".") &&
                    strcmp(entry->d_name, "..")) {
                contents.push_back(entry->d_name);
            }
        }
        closedir(directory);
#endif

        return contents;
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
        path = getenv("HOME");
        path += "/";
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

    std::string GetParentDirectory(const std::string& path) {
        for (std::size_t i = path.length() - 1; i > 0; --i) {
            if (path[i] == '\\' || path[i] == '/')
                return path.substr(0, i);
        }

        return path;
    }

    std::string GetExtension(const std::string& filename) {
        std::size_t pos = filename.find_last_of('.');
        if (pos == std::string::npos)
            return "";

        std::string extension = filename.substr(pos + 1U);
        for (std::size_t i = 0U; i < extension.length(); ++i)
            extension[i] = tolower(extension[i]);

        return extension;
    }

    std::string GetName(const std::string& filepath) {
        std::size_t start = filepath.find_last_of(DELIMITER);
        if (start == std::string::npos)
            start = 0;

        std::size_t length = filepath.find_last_of(".") - start;

        return filepath.substr(start + 1, length - 1);
    }

    std::string GetDirectory(const std::string& path) {
        std::size_t end = path.find_last_of(DELIMITER);
        if (end == std::string::npos)
            end = 0;
        else
            end++;

        return path.substr(0, end);
    }

    void ExecuteProgram(const std::string& path, const std::string& arguments) {
#if defined(_WIN32) || defined(WIN32)
        STARTUPINFO si = { 0 };
        si.cb = sizeof(si);
        PROCESS_INFORMATION pi;
        char* temp = new char[path.length() + arguments.length() + 4];
        strcpy(temp, ("\"" + path + "\" " + arguments).c_str());
        if (!CreateProcess(path.c_str(), temp, NULL, NULL, FALSE, 0, 0, 0, &si, &pi))
            Log() << "Failed to execute program.\n";
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        delete[] temp;
#else
        Log() << "Executing a program not supported on this platform.\n";
#endif
    }
}
