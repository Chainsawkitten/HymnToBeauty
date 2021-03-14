#include "Log.hpp"

#include <ostream>
#include <iostream>

using namespace std;

ostream* Log::streams[NUMBER_OF_CHANNELS];

Log::Log(const Channel channel) {
    currentChannel = channel;
}

Log::~Log() {
    streams[currentChannel]->flush();
}

Log& Log::operator<<(const string& text) {
    *streams[currentChannel] << text;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << text;
#endif

    return *this;
}

Log& Log::operator<<(const int value) {
    *streams[currentChannel] << value;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << value;
#endif

    return *this;
}

Log& Log::operator<<(const unsigned int value) {
    *streams[currentChannel] << value;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << value;
#endif

    return *this;
}

Log& Log::operator<<(const uint64_t value) {
    *streams[currentChannel] << value;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << value;
#endif

    return *this;
}

Log& Log::operator<<(const float value) {
    *streams[currentChannel] << value;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << value;
#endif

    return *this;
}

Log& Log::operator<<(const double value) {
    *streams[currentChannel] << value;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << value;
#endif

    return *this;
}

Log& Log::operator<<(const time_t value) {
    struct tm* timeinfo = localtime(&value);

    const unsigned int bufferLength = 24;

    char buffer[bufferLength] = {'\0'};
    strftime(buffer, bufferLength, "%Y-%m-%d %H:%M:%S", timeinfo);
    string const outString = string(buffer);

    *streams[currentChannel] << outString;
#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << outString;
#endif
    return *this;
}

Log& Log::operator<<(const glm::vec2& value) {
    string outString = "(" + std::to_string(value.x) + "," + std::to_string(value.y) + ")";
    *streams[currentChannel] << outString;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << outString;
#endif

    return *this;
}

Log& Log::operator<<(const glm::vec3& value) {
    string outString = "(" + std::to_string(value.x) + "," + std::to_string(value.y) + "," + std::to_string(value.z) + ")";
    *streams[currentChannel] << outString;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << outString;
#endif

    return *this;
}

Log& Log::operator<<(const glm::vec4& value) {
    string outString = "(" + std::to_string(value.x) + "," + std::to_string(value.y) + "," + std::to_string(value.z) + "," + std::to_string(value.w) + ")";
    *streams[currentChannel] << outString;

#ifdef USINGDOUBLELOGGING
    if (currentChannel != INFO)
        std::cout << outString;
#endif

    return *this;
}

bool Log::SetupStream(const Channel channel, std::ostream* stream) {
    // Check if channel is outside the range of available channels
    if (channel < DEFAULT || channel >= NUMBER_OF_CHANNELS) {
        std::cout << "Error channel: out of range.\n";
        return false;
    }

    // Set the channel.
    streams[static_cast<int>(channel)] = stream;
    return true;
}

bool Log::SetupStreams(std::ostream* defaultStream, std::ostream* info, std::ostream* warning, std::ostream* error) {
    return SetupStream(DEFAULT, defaultStream) && SetupStream(INFO, info) && SetupStream(WARNING, warning) && SetupStream(ERR, error);
}
