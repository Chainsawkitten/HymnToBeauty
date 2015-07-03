#include "Log.hpp"

#include <cstdio>

using namespace std;

Log::~Log() {
    fflush(stderr);
}

Log& Log::operator<<(string text) {
    fputs(text.c_str(), stderr);
    return *this;
}

Log& Log::operator <<(int value) {
    fprintf(stderr, "%i", value);
    return *this;
}

Log& Log::operator <<(float value) {
    fprintf(stderr, "%f", value);
    return *this;
}
