#ifndef LOG_HPP
#define LOG_HPP

#include <string>

/** @ingroup Core
 * @{
 */

/// Logging class.
/**
 * Usage:
 * @code{.cpp}
 * Log() << "Testing: " << 5 << "\n";
 * @endcode
 */
class Log {
    public:
        /// Destructor.
        ~Log();
        
        /// Output some text to stderr.
        /**
         * @param text Text to output.
         * @return The %Log instance
         */
        Log& operator<<(std::string text);
        
        /// Output an integer to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(int value);
        
        /// Output a float to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(float value);
};

/** @} */

#endif
