#include "ImageVerification.hpp"

#include <assert.h>
#include <cstdlib>
#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Utility/Log.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STBI_ONLY_PNG
#include <stb_image.h>

using namespace Video;

ImageVerification::ImageVerification(LowLevelRenderer* lowLevelRenderer, Texture* texture) {
    data = lowLevelRenderer->ReadImage(texture);
    size = texture->GetSize();
}

ImageVerification::~ImageVerification() {
    delete[] data;
}

void ImageVerification::WritePNG(const char* filename) const {
    assert(filename != nullptr);

    stbi_write_png(filename, size.x, size.y, 4, data, 4 * size.x);
}

bool ImageVerification::Compare(const char* imageData, unsigned int imageDataLength, int maxDifference) const {
    // Load reference image.
    int components, width, height;
    unsigned char* referenceData = reinterpret_cast<unsigned char*>(stbi_load_from_memory(reinterpret_cast<const unsigned char*>(imageData), imageDataLength, &width, &height, &components, 0));

    if (referenceData == NULL)
        Log() << "Couldn't load headerized image.\n";

    // Compare results to reference.
    bool match = true;
    for (uint32_t y = 0; y < size.y; ++y) {
        for (uint32_t x = 0; x < size.x; ++x) {
            for (uint32_t component = 0; component < 4; ++component) {
                int color = data[(y * size.x + x) * 4 + component];
                int reference = referenceData[(y * size.x + x) * 4 + component];
                if (abs(color - reference) > maxDifference) {
                    match = false;
                }
            }
        }
    }

    // Cleanup.
    stbi_image_free(referenceData);

    return match;
}
