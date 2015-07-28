#include "Button.hpp"
#include "Default2D.vert.hpp"
#include "Texture2D.frag.hpp"
#include "SingleColor2D.frag.hpp"
#include "../Resources.hpp"
#include "../Util/Input.hpp"

namespace GUI {
    Button::Button(Widget* parent, Texture2D* texture) : Widget(parent) {
        rectangle = Resources().CreateRectangle();
        
        vertexShader = Resources().CreateShader(DEFAULT2D_VERT, DEFAULT2D_VERT_LENGTH, GL_VERTEX_SHADER);
        colorFragmentShader = Resources().CreateShader(SINGLECOLOR2D_FRAG, SINGLECOLOR2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
        textureFragmentShader = Resources().CreateShader(TEXTURE2D_FRAG, TEXTURE2D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
        colorShaderProgram = Resources().CreateShaderProgram({ vertexShader, colorFragmentShader });
        textureShaderProgram = Resources().CreateShaderProgram({ vertexShader, textureFragmentShader });
        
        this->texture = texture;
        
        mouseHover = false;
        hasClickedMethod = false;
        size = glm::vec2(64.f, 64.f);
    }
    
    Button::~Button() {
        Resources().FreeShader(vertexShader);
        Resources().FreeShader(colorFragmentShader);
        Resources().FreeShader(textureFragmentShader);
        Resources().FreeShaderProgram(colorShaderProgram);
        Resources().FreeShaderProgram(textureShaderProgram);
        
        Resources().FreeRectangle();
    }
    
    void Button::Update() {
        double xpos = Input()->CursorX();
        double ypos = Input()->CursorY();
        
        mouseHover = xpos >= Position().x && xpos < Position().x + size.x && ypos >= Position().y && ypos < Position().y + size.y;
        
        if (mouseHover && Input()->MousePressed(GLFW_MOUSE_BUTTON_LEFT) && hasClickedMethod) {
            (Parent()->*clickedMethod)();
        }
    }
    
    void Button::Render(int screenWidth, int screenHeight) {
        // Disable depth testing
        GLboolean depthTest = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);
        
        // Blending enabled
        GLboolean blend = glIsEnabled(GL_BLEND);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Draw background
        colorShaderProgram->Use();
        
        // Set color.
        glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
        if (mouseHover)
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
        glUniform3fv(colorShaderProgram->UniformLocation("color"), 1, &color[0]);
        
        // Set location and size.
        glm::vec2 screenSize(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
        glUniform2fv(colorShaderProgram->UniformLocation("position"), 1, &(Position() / screenSize)[0]);
        glUniform2fv(colorShaderProgram->UniformLocation("size"), 1, &(size / screenSize)[0]);
        
        glBindVertexArray(rectangle->VertexArray());
        
        glDrawElements(GL_TRIANGLES, rectangle->IndexCount(), GL_UNSIGNED_INT, (void*)0);
        
        // Draw texture
        textureShaderProgram->Use();
        
        // Texture unit 0 is for base images.
        glUniform1i(textureShaderProgram->UniformLocation("baseImage"), 0);
        
        // Base image texture
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture->TextureID());
        
        // Set location and size.
        glUniform2fv(textureShaderProgram->UniformLocation("position"), 1, &(Position() / screenSize)[0]);
        glUniform2fv(textureShaderProgram->UniformLocation("size"), 1, &(size / screenSize)[0]);
        
        glBindVertexArray(rectangle->VertexArray());
        
        glDrawElements(GL_TRIANGLES, rectangle->IndexCount(), GL_UNSIGNED_INT, (void*)0);
        
        if (depthTest)
            glEnable(GL_DEPTH_TEST);
        if (!blend)
            glDisable(GL_BLEND);
    }
    
    glm::vec2 Button::Size() const {
        return size;
    }
    
    void Button::SetClickedCallback(ClickedMethod method) {
        clickedMethod = method;
        hasClickedMethod = true;
    }
}
