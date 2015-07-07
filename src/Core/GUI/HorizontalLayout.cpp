#include "HorizontalLayout.hpp"
#include "SingleColor2D.vert.hpp"
#include "SingleColor2D.frag.hpp"
#include "../Resources.hpp"

namespace GUI {
    HorizontalLayout::HorizontalLayout(Widget* parent) : Container(parent) {
        rectangle = Resources::GetInstance().CreateRectangle();
        
        vertexShader = Resources::GetInstance().CreateShader(SINGLECOLOR2D_VERT, SINGLECOLOR2D_VERT_LENGTH, GL_VERTEX_SHADER);
        fragmentShader = Resources::GetInstance().CreateShader(SINGLECOLOR2D_FRAG, SINGLECOLOR2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
        shaderProgram = Resources::GetInstance().CreateShaderProgram({ vertexShader, fragmentShader });
        
        nextPosition = glm::vec2(0.f, 0.f);
    }
    
    HorizontalLayout::~HorizontalLayout() {
        Resources::GetInstance().FreeShader(vertexShader);
        Resources::GetInstance().FreeShader(fragmentShader);
        Resources::GetInstance().FreeShaderProgram(shaderProgram);
        
        Resources::GetInstance().FreeRectangle();
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
        
        RenderWidgets(screenWidth, screenHeight);
    }
    
    void HorizontalLayout::AddWidget(Widget* widget) {
        Container::AddWidget(widget);
        widget->SetPosition(nextPosition);
        nextPosition.x += widget->Size().x;
    }
    
    glm::vec2 HorizontalLayout::Size() const {
        return this->size;
    }
    
    void HorizontalLayout::SetSize(const glm::vec2& size) {
        this->size = size;
    }
}
