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

Log& Log::operator <<(unsigned int value) {
    fprintf(stderr, "%u", value);
    return *this;
}

Log& Log::operator <<(float value) {
    fprintf(stderr, "%f", value);
    return *this;
}

Log& Log::operator <<(double value) {
    fprintf(stderr, "%f", value);
    return *this;
}

Log& Log::operator <<(time_t value) {
    struct tm * timeinfo = localtime(&value);
    
    char buffer[24];
    strftime(buffer, 24, "%Y-%m-%d %H:%M:%S", timeinfo);
    *this << buffer;
    
    return *this;
}

Log& Log::operator <<(const glm::vec2& value) {
    fprintf(stderr, "(%f, %f)", value.x, value.y);
    return *this;
}

Log& Log::operator <<(const glm::vec3& value) {
    fprintf(stderr, "(%f, %f, %f)", value.x, value.y, value.z);
    return *this;
}

Log & Log::operator<<(const glm::vec4 & value) {
    fprintf(stderr, "(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    return *this;
}

void ErrorCallback(int error, const char* description) {
	fputs(description, stderr);
}

void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	switch (source) {
	case GL_DEBUG_SOURCE_API:
		fputs("Open GL API", stderr);
		break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		fputs("Window System", stderr);
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		fputs("Shader Compiler", stderr);
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		fputs("Application", stderr);
		break;
	default:
		fputs("Other", stderr);
	}

	fputs(": ", stderr);

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		fputs("Error", stderr);
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		fputs("Deprecated Behavior", stderr);
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		fputs("Undefined Behavior", stderr);
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		fputs("Portability", stderr);
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		fputs("Performance", stderr);
		break;
	case GL_DEBUG_TYPE_MARKER:
		fputs("Marker", stderr);
		break;
	case GL_DEBUG_TYPE_PUSH_GROUP:
		fputs("Push Group", stderr);
		break;
	case GL_DEBUG_TYPE_POP_GROUP:
		fputs("Pop Group", stderr);
		break;
	default:
		fputs("Other", stderr);
	}

	fputs(" (", stderr);

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		fputs("High Priority", stderr);
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		fputs("Medium Priority", stderr);
		break;
	case GL_DEBUG_SEVERITY_LOW:
		fputs("Low Priority", stderr);
		break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		fputs("Notification", stderr);
		break;
	}

	fputs("):\n", stderr);

	fputs(message, stderr);
	fputs("\n\n", stderr);

	fflush(stderr);
}
