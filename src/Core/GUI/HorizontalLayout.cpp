#include "HorizontalLayout.hpp"
#include "test.vert.hzz"
#include "test.frag.hzz"

namespace GUI {
    HorizontalLayout::HorizontalLayout(Widget* parent) : Container(parent) {
        rectangle = new Geometry::Rectangle();
        
        vertexShader = new Shader(TEST_VERT, TEST_VERT_LENGTH, GL_VERTEX_SHADER);
        fragmentShader = new Shader(TEST_FRAG, TEST_FRAG_LENGTH, GL_FRAGMENT_SHADER);
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
        
        glBindVertexArray(rectangle->VertexArray());
        
        glDrawElements(GL_TRIANGLES, rectangle->IndexCount(), GL_UNSIGNED_INT, (void*)0);
    }
}
