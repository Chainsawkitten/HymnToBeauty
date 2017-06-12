#pragma once

#include <GL/glew.h>
#include <stb_truetype.h>
#include <glm/glm.hpp>

class Shader;
class ShaderProgram;
namespace Geometry {
    class Rectangle;
}

/// %Font used for drawing text.
class Font {
    public:
        /// Create new font.
        /**
         * @param filename Filename to TrueType file.
         * @param height Character height.
         */
        Font(const char* filename, float height);
        
        /// Create new font.
        /**
          * @param source TTF source.
          * @param sourceLength Length of source.
          * @param height Character height.
          */
        Font(const char* source, int sourceLength, float height);
        
        /// Free allocated resources.
        ~Font();
        
        /// Render a string to the screen.
        /**
         * @param text Text to render.
         * @param position Position to render the text at (in pixels).
         * @param wrap Width of text before wrapping.
         */
        void RenderText(const char* text, const glm::vec2& position, float wrap);
        
        /// Get rendering color.
        /**
         * @return Color to render text as.
         */
        const glm::vec3& GetColor() const;
        
        /// Set rendering color.
        /**
         * @param color Color to render text as.
         */
        void SetColor(const glm::vec3& color);
        
        /// Get text width.
        /**
         * @param text Text to get the rendered width of.
         * @return The width of the rendered text.
         */
        float GetWidth(const char* text);
        
        /// Get character height.
        /**
         * @return The character height.
         */
        float GetHeight() const;
        
        /// Get whether the font was created from file.
        /**
         * @return Whether the font was created from file.
         */
        bool IsFromFile() const;
        
    private:
        float RenderCharacter(char character, const glm::vec2& position);
        stbtt_aligned_quad BakedQuad(char character, float& x, float& y);
        
        bool isFromFile;
        
        GLuint texture;
        stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
        
        float height;
        glm::vec3 color = glm::vec3(0.f, 0.f, 0.f);
        
        Geometry::Rectangle* rectangle;
        
        // Shaders
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
};
