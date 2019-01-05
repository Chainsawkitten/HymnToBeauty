#include "PostProcessing.hpp"

#include "../Shader/ShaderProgram.hpp"
#include "../Geometry/Rectangle.hpp"
#include "Filter.hpp"
#include "../Shader/Shader.hpp"
#include <Post.vert.hpp>
#include "../RenderSurface.hpp"
#include "../Buffer/ReadWriteTexture.hpp"
#include "../Buffer/FrameBuffer.hpp"

using namespace Video;

PostProcessing::PostProcessing(const Geometry::Rectangle* rectangle) {
    this->rectangle = rectangle;
}

PostProcessing::~PostProcessing() {}

void PostProcessing::ApplyFilter(Video::RenderSurface* renderSurface, Video::Filter* filter) const {
    // Always pass depth test.
    glDepthFunc(GL_ALWAYS);

    // Swap render surface read/write buffers.
    renderSurface->Swap();

    // Set render surface as render target.
    renderSurface->GetColorFrameBuffer()->BindWrite();

    // Bind shaders.
    filter->GetShaderProgram()->Use();

    glUniform1i(filter->GetDiffuseLocation(), 0);
    renderSurface->GetColorTexture()->BindForReading(GL_TEXTURE0);

    glBindVertexArray(rectangle->GetVertexArray());

    filter->SetUniforms();

    glDrawElements(GL_TRIANGLES, rectangle->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);

    renderSurface->GetColorFrameBuffer()->Unbind();

    // Reset depth testing to standard value.
    glDepthFunc(GL_LESS);
}
