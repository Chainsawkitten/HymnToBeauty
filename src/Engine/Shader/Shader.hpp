#pragma once

#include <gl/glew.h>
#include <string>

/// A GLSL shader.
/**
 * Compiles and handles a GLSL shader. Shaders should be linked together into a ShaderProgram.
 */
class Shader {
	public:
		/// Compile shader.
		/**
		 * @param source GLSL code for the shader.
		 * @param sourceLength Length of the GLSL source code.
		 * @param shaderType %Shader type. One of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
		 */
		Shader(const char* source, int sourceLength, GLenum shaderType);

		/// Destructor
		~Shader();

		/// Get shader ID.
		/**
		 * Used when linking a ShaderProgram.
		 * @return Shader identifier
		 */
		GLuint ShaderID() const;

		/// Get shader type.
		/**
		 * @return %Shader type. One of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
		 */
		GLenum ShaderType() const;

	private:
		GLuint shader;
		GLenum shaderType;
};
