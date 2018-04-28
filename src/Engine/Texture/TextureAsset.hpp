#pragma once

#include <string>

namespace Video {
    class Texture2D;
}

/// A texture used in a hymn.
class TextureAsset {
    public:
        /// Create new texture asset.
        TextureAsset();

        /// Create new texture asset from a PNG image.
        /**
         * @param source Source string containing the image file.
         * @param sourceLength Length of the source string.
         */
        TextureAsset(const char* source, int sourceLength);

        /// Destructor.
        ~TextureAsset();

        /// Save the texture asset.
        void Save() const;

        /// Load texture asset from disk.
        /**
         * @param name The name of the texture asset.
         */
        void Load(const std::string& name);

        /// Get the texture.
        /**
         * @return The texture.
         */
        Video::Texture2D* GetTexture() const;

        /// The name of the texture.
        std::string name;

        /// The folder containing the texture files.
        std::string path;

    private:
        TextureAsset(const TextureAsset & other) = delete;
        Video::Texture2D* texture;
};
