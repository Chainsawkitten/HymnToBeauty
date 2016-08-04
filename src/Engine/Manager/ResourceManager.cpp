#include "ResourceManager.hpp"

#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Geometry/Rectangle.hpp"
#include "../Geometry/Cube.hpp"
#include "../Geometry/OBJModel.hpp"
#include "../Texture/Texture2D.hpp"
#include "../Font/Font.hpp"

using namespace std;

ResourceManager::ResourceManager() {
    rectangleCount = 0;
    cubeCount = 0;
}

Shader* ResourceManager::CreateShader(const char* source, int sourceLength, GLenum shaderType) {
    if (shaders.find(source) == shaders.end()) {
        shaders[source].shader = new Shader(source, sourceLength, shaderType);
        shadersInverse[shaders[source].shader] = source;
        shaders[source].count = 1;
    } else {
        shaders[source].count++;
    }
    
    return shaders[source].shader;
}

void ResourceManager::FreeShader(Shader* shader) {
    const char* source = shadersInverse[shader];
    
    shaders[source].count--;
    if (shaders[source].count <= 0) {
        shadersInverse.erase(shader);
        delete shader;
        shaders.erase(source);
    }
}

ResourceManager::ShaderProgramKey::ShaderProgramKey() {
    computeShader = nullptr;
    vertexShader = nullptr;
    tessControlShader = nullptr;
    tessEvaluationShader = nullptr;
    geometryShader = nullptr;
    fragmentShader = nullptr;
}

bool ResourceManager::ShaderProgramKey::operator<(const ShaderProgramKey& other) const {
    if (computeShader < other.computeShader) return true;
    if (computeShader > other.computeShader) return false;
    
    if (vertexShader < other.vertexShader) return true;
    if (vertexShader > other.vertexShader) return false;
    
    if (tessControlShader < other.tessControlShader) return true;
    if (tessControlShader > other.tessControlShader) return false;
    
    if (tessEvaluationShader < other.tessEvaluationShader) return true;
    if (tessEvaluationShader > other.tessEvaluationShader) return false;
    
    if (geometryShader < other.geometryShader) return true;
    if (geometryShader > other.geometryShader) return false;
    
    if (fragmentShader < other.fragmentShader) return true;
    if (fragmentShader > other.fragmentShader) return false;
    
    return false;
}

ShaderProgram* ResourceManager::CreateShaderProgram(std::initializer_list<const Shader*> shaders) {
    ShaderProgramKey key;
    
    for (auto shader : shaders) {
        switch (shader->GetShaderType()) {
        case GL_COMPUTE_SHADER:
            key.computeShader = shader;
            break;
        case GL_VERTEX_SHADER:
            key.vertexShader = shader;
            break;
        case GL_TESS_CONTROL_SHADER:
            key.tessControlShader = shader;
            break;
        case GL_TESS_EVALUATION_SHADER:
            key.tessEvaluationShader = shader;
            break;
        case GL_GEOMETRY_SHADER:
            key.geometryShader = shader;
            break;
        case GL_FRAGMENT_SHADER:
            key.fragmentShader = shader;
            break;
        }
    }
    
    if (shaderPrograms.find(key) == shaderPrograms.end()) {
        ShaderProgramInstance shaderProgram;
        shaderProgram.shaderProgram = new ShaderProgram(shaders);
        shaderProgram.count = 1;
        shaderPrograms[key] = shaderProgram;
        shaderProgramsInverse[shaderProgram.shaderProgram] = key;
    } else {
        shaderPrograms[key].count++;
    }
    
    return shaderPrograms[key].shaderProgram;
}

void ResourceManager::FreeShaderProgram(ShaderProgram* shaderProgram) {
    ShaderProgramKey key = shaderProgramsInverse[shaderProgram];
    shaderPrograms[key].count--;
    
    if (shaderPrograms[key].count <= 0) {
        shaderProgramsInverse.erase(shaderProgram);
        delete shaderProgram;
        shaderPrograms.erase(key);
    }
}

Geometry::Rectangle* ResourceManager::CreateRectangle() {
    if (rectangleCount == 0)
        rectangle = new Geometry::Rectangle();
    
    rectangleCount++;
    return rectangle;
}

void ResourceManager::FreeRectangle() {
    rectangleCount--;
    
    if (rectangleCount <= 0)
        delete rectangle;
}

Geometry::Cube* ResourceManager::CreateCube() {
    if (cubeCount++ == 0)
        cube = new Geometry::Cube();
    
    return cube;
}

void ResourceManager::FreeCube() {
    if (--cubeCount <= 0)
        delete rectangle;
}

Geometry::OBJModel* ResourceManager::CreateOBJModel(std::string filename) {
    if (objModels.find(filename) == objModels.end()) {
        objModels[filename].model = new Geometry::OBJModel(filename.c_str());
        objModelsInverse[objModels[filename].model] = filename;
        objModels[filename].count = 1;
    } else {
        objModels[filename].count++;
    }

    return objModels[filename].model;
}

void ResourceManager::FreeOBJModel(Geometry::OBJModel* model) {
    string filename = objModelsInverse[model];
    
    if (objModels[filename].count-- <= 1) {
        objModelsInverse.erase(model);
        delete model;
        objModels.erase(filename);
    }
}

Texture2D* ResourceManager::CreateTexture2D(const char* data, int dataLength, bool srgb) {
    if (textures.find(data) == textures.end()) {
        textures[data].texture = new Texture2D(data, dataLength, srgb);
        texturesInverse[textures[data].texture] = data;
        textures[data].count = 1;
    } else {
        textures[data].count++;
    }
    
    return textures[data].texture;
}

Texture2D* ResourceManager::CreateTexture2DFromFile(std::string filename, bool srgb) {
    if (texturesFromFile.find(filename) == texturesFromFile.end()) {
        texturesFromFile[filename].texture = new Texture2D(filename.c_str(), srgb);
        texturesFromFileInverse[texturesFromFile[filename].texture] = filename;
        texturesFromFile[filename].count = 1;
    } else {
        texturesFromFile[filename].count++;
    }
    
    return texturesFromFile[filename].texture;
}

void ResourceManager::FreeTexture2D(Texture2D* texture) {
    if (texture->IsFromFile()) {
        string filename = texturesFromFileInverse[texture];
        
        if (texturesFromFile[filename].count-- <= 1) {
            texturesFromFileInverse.erase(texture);
            delete texture;
            texturesFromFile.erase(filename);
        }
    } else {
        const char* data = texturesInverse[texture];
        
        if (textures[data].count-- <= 1) {
            texturesInverse.erase(texture);
            delete texture;
            textures.erase(data);
        }
    }
}

ResourceManager::FontKey::FontKey() {
    source = nullptr;
    height = 0.f;
}

bool ResourceManager::FontKey::operator<(const FontKey& other) const {
    if (source < other.source) return true;
    if (source > other.source) return false;
    
    if (height < other.height) return true;
    if (height > other.height) return false;
    
    return false;
}

Font* ResourceManager::CreateFontEmbedded(const char* source, int sourceLength, float height) {
    FontKey key;
    key.source = source;
    key.height = height;
    
    if (fonts.find(key) == fonts.end()) {
        fonts[key].font = new Font(source, sourceLength, height);
        fontsInverse[fonts[key].font] = key;
        fonts[key].count = 1;
    } else {
        fonts[key].count++;
    }
    
    return fonts[key].font;
}

ResourceManager::FontFromFileKey::FontFromFileKey() {
    filename = "";
    height = 0.f;
}

bool ResourceManager::FontFromFileKey::operator<(const FontFromFileKey& other) const {
    if (filename < other.filename) return true;
    if (filename > other.filename) return false;
    
    if (height < other.height) return true;
    if (height > other.height) return false;
    
    return false;
}

Font* ResourceManager::CreateFontFromFile(std::string filename, float height) {
    FontFromFileKey key;
    key.filename = filename;
    key.height = height;
    
    if (fontsFromFile.find(key) == fontsFromFile.end()) {
        fontsFromFile[key].font = new Font(filename.c_str(), height);
        fontsFromFileInverse[fontsFromFile[key].font] = key;
        fontsFromFile[key].count = 1;
    } else {
        fontsFromFile[key].count++;
    }
    
    return fontsFromFile[key].font;
}

void ResourceManager::FreeFont(Font* font) {
    if (font->IsFromFile()) {
        FontFromFileKey key = fontsFromFileInverse[font];
        
        if (fontsFromFile[key].count-- <= 1) {
            fontsFromFileInverse.erase(font);
            delete font;
            fontsFromFile.erase(key);
        }
    } else {
        FontKey key = fontsInverse[font];
        
        if (fonts[key].count-- <= 1) {
            fontsInverse.erase(font);
            delete font;
            fonts.erase(key);
        }
    }
}
