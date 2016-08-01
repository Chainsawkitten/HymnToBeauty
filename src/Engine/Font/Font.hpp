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
        
        /// Get quad for rendering a character.
        /**
         * The x and y positions will be updated to hold the position for the next character.
         * @param character Character.
         * @param x X-position to render at.
         * @param y Y-position to render at.
         * @return Baked quad.
         */
        stbtt_aligned_quad BakedQuad(char character, float& x, float& y);
        
        /// Render a string to the screen.
        /**
         * @param text Text to render.
         * @param position Position to render the text at (in pixels).
         * @param wrap Width of text before wrapping.
         * @param screenSize Size of the screen in pixels.
         */
        void RenderText(const char* text, const glm::vec2& position, float wrap, const glm::vec2& screenSize);
        
        /// Get rendering color.
        /**
         * @return  Color to render text as.
         */
        glm::vec3 GetColor() const;
        
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
        float RenderCharacter(char character, const glm::vec2& position, const glm::vec2& screenSize);
        
        bool isFromFile;
        
        GLuint texture;
        stbtt_bakedchar cdata[96]; // ASCII 32..126 is 95 glyphs
        
        float height;
        glm::vec3 color;
        
        Geometry::Rectangle* rectangle;
        
        // Shaders
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
};
