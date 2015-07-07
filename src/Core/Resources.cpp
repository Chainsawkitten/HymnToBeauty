#include "Resources.hpp"

Resources::Resources() {
    
}

Resources& Resources::GetInstance() {
    static Resources resources;
    
    return resources;
}

Shader* Resources::CreateShader(const char* source, int sourceLength, GLenum shaderType) {
    if (shaders.find(source) == shaders.end()) {
        shaders[source].shader = new Shader(source, sourceLength, shaderType);
        shaders[source].count = 1;
    } else {
        shaders[source].count++;
    }
    
    return shaders[source].shader;
}

void Resources::FreeShader(const char* source) {
    shaders[source].count--;
    if (shaders[source].count <= 0) {
        delete shaders[source].shader;
        shaders.erase(source);
    }
}

Geometry::Rectangle* Resources::CreateRectangle() {
    if (rectangleCount == 0)
        rectangle = new Geometry::Rectangle();
    
    rectangleCount++;
    return rectangle;
}

void Resources::FreeRectangle() {
    rectangleCount--;
    
    if (rectangleCount <= 0)
        delete rectangle;
}
