#include "Rectangle.hpp"
#include "Default2D.vert.hpp"
#include "SingleColor2D.frag.hpp"
#include <Core/Resources.hpp>

namespace Geometry {
    Rectangle::Rectangle() {
        // Vertices.
        vertexNr = 4;
        vertexData = new Vertex[vertexNr];
        
        vertexData[0] = {
            glm::vec2(-1.f, 1.f),
            glm::vec2(0.f, 1.f)
        };
        vertexData[1] = {
            glm::vec2(1.f, 1.f),
            glm::vec2(1.f, 1.f)
        };
        vertexData[2] = {
            glm::vec2(-1.f, -1.f),
            glm::vec2(0.f, 0.f)
        };
        vertexData[3] = {
            glm::vec2(1.f, -1.f),
            glm::vec2(1.f, 0.f)
        };
        
        // Vertex indices.
        indexNr = 6;
        indexData = new unsigned int[indexNr];
        
        indexData[0] = 0;
        indexData[1] = 2;
        indexData[2] = 1;
        indexData[3] = 1;
        indexData[4] = 2;
        indexData[5] = 3;
        
        GenerateBuffers();
        GenerateVertexArray();
        
        vertexShader = Resources().CreateShader(DEFAULT2D_VERT, DEFAULT2D_VERT_LENGTH, GL_VERTEX_SHADER);
        fragmentShader = Resources().CreateShader(SINGLECOLOR2D_FRAG, SINGLECOLOR2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
        shaderProgram = Resources().CreateShaderProgram({ vertexShader, fragmentShader });
    }
    
    Rectangle::~Rectangle() {
        delete[] vertexData;
        delete[] indexData;
    }
    
    Geometry2D::Vertex* Rectangle::Vertices() const {
        return vertexData;
    }
    
    unsigned int Rectangle::VertexCount() const {
        return vertexNr;
    }
    
    unsigned int* Rectangle::Indices() const {
        return indexData;
    }
    
    unsigned int Rectangle::IndexCount() const {
        return indexNr;
    }
    
    void Rectangle::Render(const glm::vec2 &position, const glm::vec2 &size, const glm::vec3 &color, int screenWidth, int screenHeight) const {
        // Disable depth testing.
        GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        
        // Draw background.
        shaderProgram->Use();
        
        // Set color.
        glUniform3fv(shaderProgram->UniformLocation("color"), 1, &color[0]);
        
        // Set location and size.
        glm::vec2 screenSize(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
        glUniform2fv(shaderProgram->UniformLocation("position"), 1, &(position / screenSize)[0]);
        glUniform2fv(shaderProgram->UniformLocation("size"), 1, &(size / screenSize)[0]);
        
        glBindVertexArray(VertexArray());
        
        glDrawElements(GL_TRIANGLES, IndexCount(), GL_UNSIGNED_INT, (void*)0);
        
        // Reset depth testing.
        if (depthTest)
            glEnable(GL_DEPTH_TEST);
    }
}
