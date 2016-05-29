#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(std::initializer_list<const Shader*> shaders) {
	shaderProgram = glCreateProgram();

	for (auto shader : shaders)
		glAttachShader(shaderProgram, shader->ShaderID());

	glLinkProgram(shaderProgram);
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(shaderProgram);
}

void ShaderProgram::Use() const {
	glUseProgram(shaderProgram);
}

GLuint ShaderProgram::AttributeLocation(const char* name) const {
	return glGetAttribLocation(shaderProgram, name);
}

GLuint ShaderProgram::UniformLocation(const char* name) const {
	return glGetUniformLocation(shaderProgram, name);
}
