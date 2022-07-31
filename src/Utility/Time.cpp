#include "Time.hpp"

#if ANDROID
#include <time.h>
#include <unistd.h>
#else
#include <GLFW/glfw3.h>
#endif

namespace Utility {

#if ANDROID
static clockid_t clock;
static uint64_t startTime;

static uint64_t GetPosixTime() {
    struct timespec t;
    clock_gettime(clock, &t);
    return static_cast<uint64_t>(t.tv_sec) * 1000000000u + static_cast<uint64_t>(t.tv_nsec);
}
#endif

void InitTime() {
#if ANDROID
    clock = CLOCK_REALTIME;

    // Check if monotonic clock is supported.
#ifdef _POSIX_MONOTONIC_CLOCK
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) == 0) {
        clock = CLOCK_MONOTONIC;
    }
#endif

    // Get start time.
    startTime = GetPosixTime();
#endif
    // GLFW should already have been initialized. No need to do anything.
}

double GetTime() {
#if ANDROID
    return static_cast<double>(GetPosixTime() - startTime) / 1000000000.0;
#else
    return glfwGetTime();
#endif
}

}
