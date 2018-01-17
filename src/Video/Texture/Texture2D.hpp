#pragma once

#include <GL/glew.h>

namespace Video {
    /// A two-dimensional texture.
    class Texture2D {
        public:
            /// Destructor
            virtual ~Texture2D();
            
            /// Get OpenGL texture ID.
            /**
             * Used when binding a texture before draw calls.
             * @return The OpenGL texture identifier
             */
            virtual GLuint GetTextureID() const = 0;
            
            /// Get whether the texture has been loaded yet.
            /**
             * @return Whether the texture has been loaded yet.
             */
            virtual bool IsLoaded() const = 0;
            
        protected:
            /// Create new texture.
            Texture2D();
                
        private:
            Texture2D(const Texture2D & other) = delete;
    };
}
