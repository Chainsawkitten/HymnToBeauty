#pragma once

#include <string>
#include <ctime>
#include <glm/glm.hpp>

/// Logging class.
/**
 * Usage:
 * @code{.cpp}
 * Log() << "Testing: " << 5 << "\n";
 * @endcode
 */
class Log {
    public:
        enum Channel {
            DEFAULT = 0,       ///< Default channel.
            INFO,              ///< Information.
            WARNING,           ///< Warnings.
            ERR,               ///< Error.
            NUMBER_OF_CHANNELS ///< Maximum number of channels, ensure this is the last element of the enum if adding channels.
        };
    
        /// Constructor.
        explicit Log(const Channel channel = DEFAULT);
    
        /// Destructor.
        ~Log();
    
        /// Output some text to stderr.
        /**
         * @param text Text to output.
         * @return The %Log instance
         */
        Log& operator<<(const std::string& text);
    
        /// Output an integer to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(const int value);
    
        /// Output an unsigned integer to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const unsigned int value);
    
        /// Output a float to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(const float value);
    
        /// Output a double to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const double value);
    
        /// Output a time to stderr.
        /**
         * Formatted Y-m-d H:M:S.
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(const time_t value);
    
        /// Output a vec2 to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const glm::vec2& value);
    
        /// Output a vec3 to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const glm::vec3& value);
    
        /// Output a vec4 to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const glm::vec4& value);
    
        /// Sets channels.
        /**
         * @param channel The channel to set.
         * @param stream The stream that will be mapped to the channel.
         * @return Whether the operation succeeded or not.
         */
        static bool SetupStream(const Channel channel, std::ostream* stream);
    
        /// Sets all channels at once.
        /**
         * @param defaultStream The stream for default output.
         * @param info The stream for information.
         * @param warning The stream for warnings.
         * @param error The stream for errors.
         * @return Whether the operation succeeded or not.
         */
        static bool SetupStreams(std::ostream* defaultStream, std::ostream* info, std::ostream* warning, std::ostream* error);

    private:
        Channel currentChannel;
        static std::ostream* streams[Channel::NUMBER_OF_CHANNELS];
};
