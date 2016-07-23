#pragma once

#include <string>
#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
        
        /// Output an unsigned integer to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(unsigned int value);
        
        /// Output a float to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(float value);
        
        /// Output a double to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(double value);
        
        /// Output a time to stderr.
        /**
         * Formatted Y-m-d H:M:S.
         * @param value Value to output.
         * @return The %Log instance
         */
        Log& operator<<(time_t value);
        
        /// Output a vec2 to stderr.
        /**
         * @param value Value to output.
         * @return The %Log instance.
         */
        Log& operator<<(const glm::vec2& value);
};

/// Handles errors by printing them to the standard error stream.
/**
 * GLFW error callback function. Prints incoming errors to the standard error stream (stderr).
 * @param error <a href="http://www.glfw.org/docs/3.0/group__errors.html">GLFW error code</a>.
 * @param description Description text of the error.
 * @see <a href="http://www.glfw.org/docs/3.0/group__error.html">GLFW error handling</a>
 */
void ErrorCallback(int error, const char* description);

/// Handles OpenGL debug messages.
/**
 * OpenGL debug message callback function. Prints debug messages to standard error stream (stderr).
 * @param source The source that produced the message.
 * @param type The type of message.
 * @param id The message's identifier.
 * @param severity The message severity (how important it is).
 * @param length Length of the message.
 * @param message A null-terminated string describing the message.
 * @param userParam User parameters set with glDebugMessageCallback().
 * @see <a href="https://www.opengl.org/wiki/Debug_Output">OpenGL Debug Output</a>
 */
void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
