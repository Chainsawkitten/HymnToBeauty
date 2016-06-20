#include "Texture2D.hpp"

#include "../Geometry/Rectangle.hpp"
#include "../Shader/ShaderProgram.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_TGA
#include <stb_image.h>

#include "../Util/Log.hpp"
#include "Default2D.vert.hpp"
#include "Texture2D.frag.hpp"
#include "../Resources.hpp"

Texture2D::Texture2D(const char* filename, bool srgb) {
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Load texture from file.
	int components;
	unsigned char* data = stbi_load(filename, &width, &height, &components, 0);

	if (data == NULL)
        Log() << "Couldn't load image " << filename << "\n";

	// Give the image to OpenGL.
	glTexImage2D(GL_TEXTURE_2D, 0, srgb ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, Format(components), GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	// When MAGnifying the image (no bigger mipmap available), use LINEAR filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Repeat texture when texture coordinates outside 0.0-1.0.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Generate mipmaps.
	glGenerateMipmap(GL_TEXTURE_2D);
    
    // For rendering.
    rectangle = Resources().CreateRectangle();
    
    vertexShader = Resources().CreateShader(DEFAULT2D_VERT, DEFAULT2D_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Resources().CreateShader(TEXTURE2D_FRAG, TEXTURE2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Resources().CreateShaderProgram({ vertexShader, fragmentShader });
    
    isFromFile = true;
}

Texture2D::Texture2D(const char *source, int sourceLength, bool srgb) {
    glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	// Load texture from file.
	int components;
	unsigned char* data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(source), sourceLength, &width, &height, &components, 0);

	if (data == NULL)
        Log() << "Couldn't load headerized image.\n";

	// Give the image to OpenGL.
	glTexImage2D(GL_TEXTURE_2D, 0, srgb ? GL_SRGB_ALPHA : GL_RGBA, width, height, 0, Format(components), GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);

	// When MAGnifying the image (no bigger mipmap available), use LINEAR filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Repeat texture when texture coordinates outside 0.0-1.0.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Generate mipmaps, by the way.
	glGenerateMipmap(GL_TEXTURE_2D);
    
    // For rendering.
    rectangle = Resources().CreateRectangle();
    
    vertexShader = Resources().CreateShader(DEFAULT2D_VERT, DEFAULT2D_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Resources().CreateShader(TEXTURE2D_FRAG, TEXTURE2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Resources().CreateShaderProgram({ vertexShader, fragmentShader });
    
    isFromFile = false;
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &texID);
    
    Resources().FreeShader(vertexShader);
    Resources().FreeShader(fragmentShader);
    Resources().FreeShaderProgram(shaderProgram);
    
    Resources().FreeRectangle();
}

GLuint Texture2D::GetTextureID() const {
	return texID;
}

int Texture2D::GetWidth() const {
	return width;
}

int Texture2D::GetHeight() const {
	return height;
}

void Texture2D::SetWrapping(GLint wrapMode) {
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
}

void Texture2D::Render(const glm::vec2 &position, const glm::vec2 &size, const glm::vec2& screenSize) const {
    // Disable depth testing.
    GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
    glDisable(GL_DEPTH_TEST);
    
    // Blending enabled.
    GLboolean blend = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    shaderProgram->Use();
    
    // Texture unit 0 is for base images.
    glUniform1i(shaderProgram->GetUniformLocation("baseImage"), 0);
    
    // Base image texture
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texID);
    
    // Set location and size.
    glUniform2fv(shaderProgram->GetUniformLocation("position"), 1, &(position / screenSize)[0]);
    glUniform2fv(shaderProgram->GetUniformLocation("size"), 1, &(size / screenSize)[0]);
    
    glBindVertexArray(rectangle->GetVertexArray());
    
    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    
    // Reset depth testing and blending.
    if (depthTest)
        glEnable(GL_DEPTH_TEST);
    if (!blend)
        glDisable(GL_BLEND);
}

bool Texture2D::IsFromFile() const {
    return isFromFile;
}
