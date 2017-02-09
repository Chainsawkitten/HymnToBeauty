#include "PostProcessing.hpp"

#include "../RenderTarget.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Geometry/Rectangle.hpp"
#include "Filter.hpp"
#include "../MainWindow.hpp"


PostProcessing::PostProcessing() {

    buffers[0] = new RenderTarget(MainWindow::GetInstance()->GetSize());
    buffers[1] = new RenderTarget(MainWindow::GetInstance()->GetSize());
    
    rectangle = Managers().resourceManager->CreateRectangle();

}

PostProcessing::~PostProcessing() {
    delete buffers[0];
    delete buffers[1];
    
    Managers().resourceManager->FreeRectangle();
}

RenderTarget* PostProcessing::GetRenderTarget() const {
    return buffers[which];
}

void PostProcessing::UpdateBufferSize(){

    delete buffers[0];
    delete buffers[1];

    buffers[0] = new RenderTarget(MainWindow::GetInstance()->GetSize());
    buffers[1] = new RenderTarget(MainWindow::GetInstance()->GetSize());

    rectangle = Managers().resourceManager->CreateRectangle();

}

void PostProcessing::ApplyFilter(Filter* filter) {
    // Always pass depth test.
    glDepthFunc(GL_ALWAYS);
    
    buffers[1 - which]->SetTarget();
    
    filter->GetShaderProgram()->Use();
    
    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tDiffuse"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffers[which]->GetColorTexture());
    
    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tExtra"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, buffers[which]->GetExtraTexture());
    
    glUniform1i(filter->GetShaderProgram()->GetUniformLocation("tDepth"), 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, buffers[which]->GetDepthTexture());
    
    glBindVertexArray(rectangle->GetVertexArray());
    
    filter->SetUniforms();
    
    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    
    // Reset depth testing to standard value.
    glDepthFunc(GL_LESS);
    
    which = 1 - which;
}

void PostProcessing::Render(bool dither) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    buffers[which]->Render(dither);
}
