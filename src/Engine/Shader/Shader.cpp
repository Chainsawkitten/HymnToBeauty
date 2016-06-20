#include "Shader.hpp"

#include <vector>
#include "../Util/Log.hpp"

Shader::Shader(const char* source, int sourceLength, GLenum shaderType) {
	this->shaderType = shaderType;

	shader = glCreateShader(shaderType);
    GLint length = sourceLength;
	glShaderSource(shader, 1, &source, &length);
	glCompileShader(shader);
    
    // Check for shader compilation errors.
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    
        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
        
        Log() << std::string(infoLog.begin(), infoLog.end());
    }
}

Shader::~Shader() {
	glDeleteShader(shader);
}

GLuint Shader::GetShaderID() const {
	return shader;
}

GLenum Shader::GetShaderType() const {
	return shaderType;
}
