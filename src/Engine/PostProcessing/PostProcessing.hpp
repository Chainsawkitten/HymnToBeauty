#pragma once

class RenderTarget;
class Filter;
namespace Geometry {
    class Rectangle;
}

/// Applies post-processing effects to the rendered image.
class PostProcessing {
    public:
        /// Create new post-processing handler.
        PostProcessing();
        
        /// Destructor.
        ~PostProcessing();
        
        /// Get render target.
        /**
         * @return Target to render to´.
         */
        RenderTarget* GetRenderTarget() const;
        
        /// Apply a filter to the render image.
        /**
         * filter %Filter to apply.
         */
        void ApplyFilter(Filter* filter);
        
        /// Render resulting image to screen.
        /**
         * @param dither Whether to use dithering.
         */
        void Render(bool dither = false);
        
    private:
        short int which = 0;
        
        RenderTarget* buffers[2];
        
        Geometry::Rectangle* rectangle;
};
