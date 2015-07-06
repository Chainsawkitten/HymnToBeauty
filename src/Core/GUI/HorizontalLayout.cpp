#include "HorizontalLayout.hpp"
#include "SingleColor2D.vert.hzz"
#include "SingleColor2D.frag.hzz"

namespace GUI {
    HorizontalLayout::HorizontalLayout(Widget* parent) : Container(parent) {
        rectangle = new Geometry::Rectangle();
        
        vertexShader = new Shader(SINGLECOLOR2D_VERT, SINGLECOLOR2D_VERT_LENGTH, GL_VERTEX_SHADER);
        fragmentShader = new Shader(SINGLECOLOR2D_FRAG, SINGLECOLOR2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
        shaderProgram = new ShaderProgram({ vertexShader, fragmentShader });
        
        testButton = new Button(this);
    }
    
    HorizontalLayout::~HorizontalLayout() {
        delete vertexShader;
        delete fragmentShader;
        delete shaderProgram;
        
        delete rectangle;
        
        delete testButton;
    }
    
    void HorizontalLayout::Update(GLFWwindow* window) {
        testButton->Update(window);
    }
    
    void HorizontalLayout::Render(int screenWidth, int screenHeight) {
        shaderProgram->Use();
        
        // Set color.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        glUniform3fv(shaderProgram->UniformLocation("color"), 1, &color[0]);
        
        // Set location and size.
        glUniform2fv(shaderProgram->UniformLocation("position"), 1, &Position()[0]);
        glUniform2fv(shaderProgram->UniformLocation("size"), 1, &size[0]);
        
        // Set screen size.
        glm::vec2 screenSize(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
        glUniform2fv(shaderProgram->UniformLocation("screenSize"), 1, &screenSize[0]);
        
        glBindVertexArray(rectangle->VertexArray());
        
        glDrawElements(GL_TRIANGLES, rectangle->IndexCount(), GL_UNSIGNED_INT, (void*)0);
        
        testButton->Render(screenWidth, screenHeight);
    }
    
    const glm::vec2& HorizontalLayout::Size() const {
        return this->size;
    }
    
    void HorizontalLayout::SetSize(const glm::vec2& size) {
        this->size = size;
    }
}
