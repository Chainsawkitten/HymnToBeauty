#include "ImageButton.hpp"
#include "Default2D.vert.hpp"
#include "SingleColor2D.frag.hpp"
#include <Core/Resources.hpp>
#include <Core/Util/Input.hpp>

namespace GUI {
    ImageButton::ImageButton(Widget* parent, Texture2D* texture) : Button(parent) {
        rectangle = Resources().CreateRectangle();
        
        vertexShader = Resources().CreateShader(DEFAULT2D_VERT, DEFAULT2D_VERT_LENGTH, GL_VERTEX_SHADER);
        fragmentShader = Resources().CreateShader(SINGLECOLOR2D_FRAG, SINGLECOLOR2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
        shaderProgram = Resources().CreateShaderProgram({ vertexShader, fragmentShader });
        
        this->texture = texture;
    }
    
    ImageButton::~ImageButton() {
        Resources().FreeShader(vertexShader);
        Resources().FreeShader(fragmentShader);
        Resources().FreeShaderProgram(shaderProgram);
        
        Resources().FreeRectangle();
    }
    
    void ImageButton::Render(int screenWidth, int screenHeight) {
        // Disable depth testing
        GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        
        // Blending enabled
        GLboolean blend = glIsEnabled(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Draw background
        shaderProgram->Use();
        
        // Set color.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        if (MouseHover())
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
        glUniform3fv(shaderProgram->UniformLocation("color"), 1, &color[0]);
        
        // Set location and size.
        glm::vec2 screenSize(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
        glUniform2fv(shaderProgram->UniformLocation("position"), 1, &(Position() / screenSize)[0]);
        glUniform2fv(shaderProgram->UniformLocation("size"), 1, &(Size() / screenSize)[0]);
        
        glBindVertexArray(rectangle->VertexArray());
        
        glDrawElements(GL_TRIANGLES, rectangle->IndexCount(), GL_UNSIGNED_INT, (void*)0);
        
        texture->Render(Position(), Size(), screenWidth, screenHeight);
        
        if (depthTest)
            glEnable(GL_DEPTH_TEST);
        if (!blend)
            glDisable(GL_BLEND);
    }
}
