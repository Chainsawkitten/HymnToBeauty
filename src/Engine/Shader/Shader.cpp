#include "Shader.hpp"

Shader::Shader(const char* source, int sourceLength, GLenum shaderType) {
	this->shaderType = shaderType;

	shader = glCreateShader(shaderType);
    GLint length = sourceLength;
	glShaderSource(shader, 1, &source, &length);
	glCompileShader(shader);
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
