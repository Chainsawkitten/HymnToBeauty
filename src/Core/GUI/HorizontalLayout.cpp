#include "HorizontalLayout.hpp"
#include "SingleColor2D.vert.hzz"
#include "SingleColor2D.frag.hzz"

namespace GUI {
    HorizontalLayout::HorizontalLayout(Widget* parent) : Container(parent) {
        rectangle = new Geometry::Rectangle();
        
        vertexShader = new Shader(SINGLECOLOR2D_VERT, SINGLECOLOR2D_VERT_LENGTH, GL_VERTEX_SHADER);
        fragmentShader = new Shader(SINGLECOLOR2D_FRAG, SINGLECOLOR2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
        shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
    }
    
    HorizontalLayout::~HorizontalLayout() {
        delete vertexShader;
        delete fragmentShader;
        delete shaderProgram;
        
        delete rectangle;
    }
    
    void HorizontalLayout::Render(int screenWidth, int screenHeight) {
        shaderProgram->Use();
        
        // Set color.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        glUniform3fv(shaderProgram->UniformLocation("color"), 1, &color[0]);
        
        // Set location and size.
        glm::vec2 topLeft(0.f, 0.f);
        glUniform2fv(shaderProgram->UniformLocation("topLeft"), 1, &topLeft[0]);
        glm::vec2 bottomRight(static_cast<float>(screenWidth), 64.f);
        glUniform2fv(shaderProgram->UniformLocation("bottomRight"), 1, &bottomRight[0]);
        
        // Set screen size.
        glm::vec2 screenSize(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
        glUniform2fv(shaderProgram->UniformLocation("screenSize"), 1, &screenSize[0]);
        
        glBindVertexArray(rectangle->VertexArray());
        
        glDrawElements(GL_TRIANGLES, rectangle->IndexCount(), GL_UNSIGNED_INT, (void*)0);
    }
}
