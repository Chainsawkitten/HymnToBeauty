#include "Log.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

#if __ANDROID__
#include <android/log.h>
#endif

using namespace std;

static bool callbackSet = false;
static std::function<void(const Log::Channel channel, const std::string& message)> callbackFunction;

static ofstream fileStream;
static bool fileSet = false;

Log::Log(const Channel channel) {
    assert(channel < NUMBER_OF_CHANNELS);

    currentChannel = channel;
}

Log::~Log() {
    // Call the callback function (if any is set).
    if (callbackSet)
        callbackFunction(currentChannel, message.str());

    // Write the message to the log file (if one is open).
    if (fileSet)
        fileStream << message.str();

    // Output the message to standard output as well.
#if __ANDROID__
    android_LogPriority priority;
    switch (currentChannel) {
    case INFO:
        priority = ANDROID_LOG_INFO;
        break;
    case WARNING:
        priority = ANDROID_LOG_WARN;
        break;
    case ERR:
        priority = ANDROID_LOG_ERROR;
        break;
    default:
        priority = ANDROID_LOG_DEFAULT;
        break;
    }

    __android_log_print(priority, "Hymn to Beauty", "%s", message.str().c_str());
#else
    ostream* str;
    switch (currentChannel) {
    case WARNING:
    case ERR:
        str = &std::cerr;
        break;
    default:
        str = &std::cout;
        break;
    }
    *str << message.str();
    str->flush();
#endif
}

Log& Log::operator<<(const string& text) {
    message << text;
    return *this;
}

Log& Log::operator<<(const int value) {
    message << value;
    return *this;
}

Log& Log::operator<<(const unsigned int value) {
    message << value;
    return *this;
}

Log& Log::operator<<(const unsigned long value) {
    message << value;
    return *this;
}

Log& Log::operator<<(const unsigned long long value) {
    message << value;
    return *this;
}

Log& Log::operator<<(const float value) {
    message << value;
    return *this;
}

Log& Log::operator<<(const double value) {
    message << value;
    return *this;
}

Log& Log::operator<<(const time_t value) {
    struct tm* timeinfo = localtime(&value);

    const unsigned int bufferLength = 24;

    char buffer[bufferLength] = {'\0'};
    strftime(buffer, bufferLength, "%Y-%m-%d %H:%M:%S", timeinfo);
    string const outString = string(buffer);

    message << outString;
    return *this;
}

Log& Log::operator<<(const glm::vec2& value) {
    message << "(" << std::to_string(value.x) << "," << std::to_string(value.y) << ")";
    return *this;
}

Log& Log::operator<<(const glm::vec3& value) {
    message << "(" << std::to_string(value.x) << "," << std::to_string(value.y) << "," << std::to_string(value.z) << ")";
    return *this;
}

Log& Log::operator<<(const glm::vec4& value) {
    message << "(" << std::to_string(value.x) << "," << std::to_string(value.y) << "," << std::to_string(value.z) << "," << std::to_string(value.w) << ")";
    return *this;
}

void Log::SetupCallback(std::function<void(const Channel channel, const std::string& message)> callback) {
    callbackFunction = callback;
    callbackSet = true;
}

void Log::ResetCallback() {
    callbackSet = false;
}

void Log::SetupFile(const std::string& filename) {
    CloseFile();
    fileStream.open(filename.c_str(), std::ofstream::app);
    fileSet = true;
}

void Log::CloseFile() {
    if (fileSet)
        fileStream.close();
    fileSet = false;
}
