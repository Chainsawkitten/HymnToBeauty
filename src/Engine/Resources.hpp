#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include "Shader/ShaderProgram.hpp"
#include <map>
#include "Geometry/Rectangle.hpp"
#include "Texture/Texture2D.hpp"
#include "Font/Font.hpp"

/// Handles all resources.
class ResourceManager {
    public:
        /// Get the instance of the class.
        /**
         * @return The %ResourceManager instance
         */
        static ResourceManager& GetInstance();
        
        /// Create a shader if it doesn't already exist.
        /**
		 * @param source GLSL code for the shader.
		 * @param sourceLength Length of the GLSL source code.
		 * @param shaderType %Shader type. One of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
		 * @return The shader instance
		 */
        Shader* CreateShader(const char* source, int sourceLength, GLenum shaderType);
        
        /// Free the reference to the shader.
        /**
         * Deletes the instance if no more references exist.
         * @param shader %Shader to dereference.
         */
        void FreeShader(Shader* shader);
        
        /// Create shader program if it doesn't already exist.
		/**
		 * Link together shaders into a shader program that can be run on the GPU.
		 *
		 * Sample:
		 * \code{.cpp}
		 * Shader* vertexShader = new Shader(vertexSource, vertexSourceLength, GL_VERTEX_SHADER);
		 * Shader* geometryShader = new Shader(geometrySource, geometrySourceLength, GL_GEOMETRY_SHADER);
		 * Shader* fragmentShader = new Shader(fragmentSource, fragmentSourceLength, GL_FRAGMENT_SHADER);
		 * ShaderProgram* shaderProgram = new ResourceManager::GetInstance().CreateShaderProgram({ vertexShader, geometryShader, fragmentShader });
		 * \endcode
		 *
		 * @param shaders List of shaders to link together.
		 * @return The shader program instance
		 */
		ShaderProgram* CreateShaderProgram(std::initializer_list<const Shader*> shaders);
        
        /// Free the reference to a shader program.
        /**
         * Deletes the instance if no more references exist.
         * @param shaderProgram %Shader program to dereference.
         */
        void FreeShaderProgram(ShaderProgram* shaderProgram);
        
        /// Create a rectangle for rendering if it doesn't already exist.
        /**
         * @return The rectangle instance
         */
        Geometry::Rectangle* CreateRectangle();
        
        /// Free the reference to the rectangle.
        /**
         * Deletes the instance if no more references exist.
         */
        void FreeRectangle();
        
        /// Create a 2D texture if it doesn't already exist.
        /**
		 * @param data Image file data.
		 * @param dataLength Length of the image file data.
		 * @return The %Texture2D instance
		 */
        Texture2D* CreateTexture2D(const char* data, int dataLength);
        
        /// Free the reference to the 2D texture.
        /**
         * Deletes the instance if no more references exist.
         * @param texture %Texture to dereference.
         */
        void FreeTexture2D(Texture2D* texture);
        
        /// Create a 2D texture if it doesn't already exist.
        /**
		 * @param filename Filename of image file.
		 * @return The %Texture2D instance
		 */
        Texture2D* CreateTexture2DFromFile(std::string filename);
        
        /// Free the reference to the 2D texture.
        /**
         * Deletes the instance if no more references exist.
         * @param texture %Texture to dereference.
         */
        void FreeTexture2DFromFile(Texture2D* texture);
        
        /// Create a font if it doesn't already exist.
        /**
         * @param source TTF source.
         * @param sourceLength Length of the source.
         * @param height Character height.
         * @return The %Font instance
         */
        Font* CreateFontEmbedded(const char* source, int sourceLength, float height);
        
        /// Free the reference to the font.
        /**
         * Deletes the instance if no more references exist.
         * @param font %Font to dereference.
         */
        void FreeFont(Font* font);
        
        /// Create a font if it doesn't already exist.
        /**
         * @param filename Filename of the TTF file.
         * @param height Character height.
         * @return The %Font instance
         */
        Font* CreateFontFromFile(std::string filename, float height);
        
        /// Free the reference to the font.
        /**
         * Deletes the instance if no more references exist.
         * @param font %Font to dereference.
         */
        void FreeFontFromFile(Font* font);
        
    private:
        ResourceManager();
        ResourceManager(ResourceManager const&) = delete;
        void operator=(ResourceManager const&) = delete;
        
        // Shaders
        struct ShaderInstance {
            Shader* shader;
            int count;
        };
        std::map<const char*, ShaderInstance> shaders;
        std::map<Shader*, const char*> shadersInverse;
        
        // ShaderPrograms
        struct ShaderProgramInstance {
            ShaderProgram* shaderProgram;
            int count;
        };
        struct ShaderProgramKey {
            const Shader* computeShader;
            const Shader* vertexShader;
            const Shader* tessControlShader;
            const Shader* tessEvaluationShader;
            const Shader* geometryShader;
            const Shader* fragmentShader;
            
            ShaderProgramKey();
            
            bool operator<(const ShaderProgramKey& other) const;
        };
        std::map<ShaderProgramKey, ShaderProgramInstance> shaderPrograms;
        std::map<ShaderProgram*, ShaderProgramKey> shaderProgramsInverse;
        
        // Rectangle
        Geometry::Rectangle* rectangle;
        int rectangleCount;
        
        // Texture2D
        struct Texture2DInstance {
            Texture2D* texture;
            int count;
        };
        std::map<const char*, Texture2DInstance> textures;
        std::map<Texture2D*, const char*> texturesInverse;
        
        // Texture2D from file
        std::map<std::string, Texture2DInstance> texturesFromFile;
        std::map<Texture2D*, std::string> texturesFromFileInverse;
        
        // Font
        struct FontInstance {
            Font* font;
            int count;
        };
        struct FontKey {
            const char* source;
            float height;
            
            FontKey();
            
            bool operator<(const FontKey& other) const;
        };
        std::map<FontKey, FontInstance> fonts;
        std::map<Font*, FontKey> fontsInverse;
        
        // Font from file
        struct FontFromFileKey {
            std::string filename;
            float height;
            
            FontFromFileKey();
            
            bool operator<(const FontFromFileKey& other) const;
        };
        std::map<FontFromFileKey, FontInstance> fontsFromFile;
        std::map<Font*, FontFromFileKey> fontsFromFileInverse;
};

/// Get the resource manager.
/**
 * @return The %ResourceManager instance
 */
ResourceManager& Resources();

#endif
