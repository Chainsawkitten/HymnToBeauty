#include "Shader.hpp"
#include "../Util/Log.hpp"

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

GLuint Shader::ShaderID() const {
	return shader;
}

GLenum Shader::ShaderType() const {
	return shaderType;
}
