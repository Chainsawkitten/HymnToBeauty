#pragma once

#include <glad/glad.h>
#include <string>

/// Run video error check.
class VideoErrorCheck {
    public:
        /// Start video error check.
        /**
         * @param name Name the scope to check for errors.
         */
        explicit VideoErrorCheck(const std::string& name);

        /// End video error check.
        ~VideoErrorCheck();

    private:
        void CheckError(const char* message);
        static const char* GetErrorString(GLenum error);

        std::string name;
};

#define VIDEO_ERROR_CHECK(name) VideoErrorCheck __videoErrorCheckInstance(name)
