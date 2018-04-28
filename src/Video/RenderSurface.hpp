#pragma once

#include <glm/glm.hpp>

namespace Video {

    class FrameBuffer;
    class ReadWriteTexture;

    /// Render surface containing framebuffers.
    class RenderSurface {
        public:
            /// Create new render surface.
            /**
             * @param size Size of the new render surface.
             */
            explicit RenderSurface(const glm::vec2& size);

            /// Destructor.
            ~RenderSurface();

            /// Get size of the render target.
            /**
            * @return The size of the render target in pixels.
            */
            glm::vec2 GetSize() const;

            /// Get depth frame buffer.
            /**
             * @return Depth frame buffer.
             */
            FrameBuffer* GetDepthFrameBuffer() const;

            /// Get shading frame buffer.
            /**
             * @return Shading frame buffer.
             */
            FrameBuffer* GetShadingFrameBuffer() const;

            /// Get color frame buffer.
            /**
             * @return Color rendering frame buffer.
             */
            FrameBuffer* GetColorFrameBuffer() const;

            /// Get texture contaning depth pixel data.
            /**
             * @return The depth texture.
             */
            ReadWriteTexture* GetDepthTexture() const;

            /// Get texture contaning color pixel data.
            /**
             * @return The color texture.
             */
            ReadWriteTexture* GetColorTexture() const;

            /// Swap active texture set.
            void Swap();

            /// Clear all textures in RenderSurface.
            void Clear() const;

        private:
            RenderSurface(const RenderSurface & other) = delete;
            glm::vec2 size;

            // Frame buffers.
            FrameBuffer* depthFrameBuffer;
            FrameBuffer* colorFrameBuffer[2];
            FrameBuffer* shadingFrameBuffer[2];

            // Textures.
            ReadWriteTexture* depthTexture;
            ReadWriteTexture* colorTexture[2];

            short int which = 0;
    };
}
