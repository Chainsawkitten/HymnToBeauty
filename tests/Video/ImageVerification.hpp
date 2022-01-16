#pragma once

#include <glm/glm.hpp>

namespace Video {
class LowLevelRenderer;
class Texture;
}

/// Helper class that does image verification against a ground truth.
class ImageVerification {
  public:
    /// Create new image verification of the given render texture.
    /**
     * @param lowLevelRenderer The low-level renderer.
     * @param texture The texture to compare against something.
     */
    ImageVerification(Video::LowLevelRenderer* lowLevelRenderer, Video::Texture* texture);

    /// Destructor.
    ~ImageVerification();

    /// Write image to PNG file.
    /**
     * @param filename Name of the file to write to.
     */
    void WritePNG(const char* filename) const;

    /// Compare the results to a reference image.
    /**
     * @param imageData Reference image data.
     * @param imageDataLength Length of the reference image data.
     * @param maxDifference The maximum difference per component to be considered a match.
     *
     * @return Whether the output matches the reference image.
     */
    bool Compare(const char* imageData, unsigned int imageDataLength, int maxDifference = 1) const;

  private:
    unsigned char* data;
    glm::uvec2 size;
};
