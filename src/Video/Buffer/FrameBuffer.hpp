#pragma once

#include <glad/glad.h>
#include <vector>

namespace Video {
    class ReadWriteTexture;

    /// Frame buffer containing frame buffer object.
    class FrameBuffer {
        public:
            /// Create new frame buffer.
            /**
             * @param textures Vector of %ReadWriteTexture to create a frame buffer object.
             */
            explicit FrameBuffer(const std::vector<ReadWriteTexture*>& textures);

            /// Destructor.
            ~FrameBuffer();

            /// Bind this framebuffer as render target.
            void BindWrite();

            /// Bind this framebuffer as render source.
            void BindRead();

            /// Unbind this framebuffer as render target.
            void Unbind();

            /// Clear this framebuffer.
            void Clear() const;

        private:
            std::vector<ReadWriteTexture*> textures;

            GLuint frameBufferObject;
            GLenum target;

            bool bound;
    };
}
