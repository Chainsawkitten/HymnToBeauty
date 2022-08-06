#pragma once

#include <string>
#include <sstream>
#include <ctime>
#include <glm/glm.hpp>
#include <functional>

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
        ERR,             ///< Error.
        NUMBER_OF_CHANNELS ///< Maximum number of channels, ensure this is the last element of the enum if adding channels.
    };

    /// Constructor.
    explicit Log(const Channel channel = DEFAULT);

    /// Destructor.
    ~Log();

    /// Output some text.
    /**
     * @param text Text to output.
     * @return The %Log instance
     */
    Log& operator<<(const std::string& text);

    /// Output an integer.
    /**
     * @param value Value to output.
     * @return The %Log instance
     */
    Log& operator<<(const int value);

    /// Output an unsigned integer.
    /**
     * @param value Value to output.
     * @return The %Log instance.
     */
    Log& operator<<(const unsigned int value);

    /// Output an unsigned long.
    /**
     * @param value Value to output.
     * @return The %Log instance.
     */
    Log& operator<<(const unsigned long value);

    /// Output an unsigned long long.
    /**
     * @param value Value to output.
     * @return The %Log instance.
     */
    Log& operator<<(const unsigned long long value);

    /// Output a float.
    /**
     * @param value Value to output.
     * @return The %Log instance
     */
    Log& operator<<(const float value);

    /// Output a double.
    /**
     * @param value Value to output.
     * @return The %Log instance.
     */
    Log& operator<<(const double value);

    /// Output a time.
    /**
     * Formatted Y-m-d H:M:S.
     * @param value Value to output.
     * @return The %Log instance
     */
    Log& operator<<(const time_t value);

    /// Output a vec2.
    /**
     * @param value Value to output.
     * @return The %Log instance.
     */
    Log& operator<<(const glm::vec2& value);

    /// Output a vec3.
    /**
     * @param value Value to output.
     * @return The %Log instance.
     */
    Log& operator<<(const glm::vec3& value);

    /// Output a vec4.
    /**
     * @param value Value to output.
     * @return The %Log instance.
     */
    Log& operator<<(const glm::vec4& value);

    /// Setup callback.
    /**
     * @param callback Callback function to call whenever something is logged.
     * @return Whether the operation succeeded.
     */
    static void SetupCallback(std::function<void(const Channel channel, const std::string& message)> callback);

    /// Remove the callback function.
    static void ResetCallback();

    /// Setup file to log to.
    /**
     * @param filename The file to write to.
     */
    static void SetupFile(const std::string& filename);

    /// Close any open logging file.
    static void CloseFile();

  private:
    Channel currentChannel;
    std::stringstream message;
};
