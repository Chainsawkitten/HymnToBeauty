#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Lighting/Light.hpp"

namespace Video {
    class StaticRenderProgram;
    class SkinRenderProgram;
    class Texture2D;
    class PostProcessing;
    class FXAAFilter;
    class ShaderProgram;
    class RenderSurface;
    class StorageBuffer;
    namespace Geometry {
        class Geometry3D;
        class Rectangle;
    } // namespace Geometry

    /// Handles rendering using OpenGL.
    class Renderer {
        public:
            /// Create new renderer.
            Renderer();

            /// Destructor.
            ~Renderer();

            /// Start rendering the frame.
            /**
             * @param renderSurface %RenderSurface to render to.
             */
            void StartRendering(RenderSurface* renderSurface);

            /// Prepare for shadow rendering static meshes.
            /**
             * @param lightView The lights view matrix
             * @param lightProjection the Lights projection matrix
             * @param shadowId the texture ID for the shadowMap
             * @param shadowMapSize The size of the shadow map.
             * @param depthFbo framebuffer object for shadowMap
             */
            void PrepareStaticShadowRendering(const glm::mat4 lightView, glm::mat4 lightProjection, int shadowId, unsigned int shadowMapSize, int dephtFbo);

            /// Render a static shadow mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             */
            void ShadowRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix);

            /// Prepare for depth rendering static meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            void PrepareStaticMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

            /// Render a static mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             */
            void DepthRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix);

            /// Prepare for rendering static meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param cameraNear Camera near plane distance.
             * @param cameraFar Camera far plane distance.
             */
            void PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float cameraNear, float cameraFar);

            /// Render a static mesh.
            /**
             * @param geometry The geometry to render.
             * @param albedo Albedo texture.
             * @param normal Normal map.
             * @param metallic Metallic map.
             * @param roughness Roughness texture.
             * @param modelMatrix Model matrix.
             */
            void RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix);

            /// Prepare for shadow rendering skin meshes.
            /**
             * @param lightView The lights view matrix
             * @param lightProjection the Lights projection matrix
             * @param shadowId the texture ID for the shadowMap
             * @param shadowMapSize The size of the shadow map.
             * @param depthFbo framebuffer object for shadowMap
            */
            void PrepareSkinShadowRendering(const glm::mat4 lightView, glm::mat4 lightProjection, int shadowId, unsigned int shadowMapSize, int dephtFbo);

            /// Prepare for depth rendering skin meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            void PrepareSkinMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

            /// Render a skin mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             * @param bones Bones array.
             */
            void DepthRenderSkinMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones);

            /// Prepare for rendering skin meshes.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param cameraNear Camera near plane distance.
             * @param cameraFar Camera far plane distance.
             */
            void PrepareSkinMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float cameraNear, float cameraFar);

            /// Render a skin shadow mesh.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             * @param bones Bones array.
             */
            void ShadowRenderSkinMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones);

            /// Render a skin mesh.
            /**
             * @param geometry The geometry to render.
             * @param albedo Albedo texture.
             * @param normal Normal map.
             * @param metallic Metallic map.
             * @param roughness Roughness texture.
             * @param modelMatrix Model matrix.
             * @param bones Bones array.
             */
            void RenderSkinMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix, const std::vector<glm::mat4>& bones);

            /// Update light buffer.
            /**
             * @param lights Vector of lights to push to the light buffer.
             */
            void SetLights(const std::vector<Video::Light>& lights);

            /// Anti-alias using FXAA.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             */
            void AntiAlias(RenderSurface* renderSurface);

            /// Render fog.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             * @param projectionMatrix The camera's projection matrix.
             * @param density The density of the fog.
             * @param color Color.
             */
            void RenderFog(RenderSurface* renderSurface, const glm::mat4& projectionMatrix, float density, const glm::vec3& color);

            /// Apply glow effect.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             * @param blurAmount How many times to blur the glow buffer.
             */
            void ApplyGlow(RenderSurface* renderSurface, int blurAmount);

            /// Apply a color filter.
            /**
             * @param renderSurface %RenderSurface to apply filter to.
             * @param color Color.
             */
            void ApplyColorFilter(RenderSurface* renderSurface, const glm::vec3& color);

            /// Display the rendered results to back buffer.
            /**
             * @param renderSurface RenderSurface to present to back buffer.
             * @param targetSize Size of the back buffer in pixels.
             */
            void Present(RenderSurface* renderSurface, const glm::vec2& targetSize);

            /// Begin rendering icons.
            /**
             * \note
             * Needs to be called before RenderIcon.
             * @param viewProjectionMatrix The camera's view projection matrix.
             * @param cameraPosition The camera's position.
             * @param cameraUp The camera's up vector.
             */
            void PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp);

            /// Render a billboarded icon.
            /**
             * \note
             * PrepareRenderingIcons must be called before.
             * @param position World position to render at.
             * @param icon The icon to render.
             */
            void RenderIcon(const glm::vec3& position, const Texture2D* icon);

            /// Stop rendering icons.
            /**
             * \note
             * Should be called after all icons have been rendered.
             */
            void StopRenderingIcons();

            /// Gamma correction.
            /**
             * @param gamma Gamma to apply.
             */
            void SetGamma(float gamma);

            /// Gamma correction.
            /**
             * @return gamma Gamma value.
             */
            float GetGamma() const;

            /// Set whether fog is applied.
            /**
             * @param fogApply Whether to apply fog.
             */
            void SetFogApply(bool fogApply);

            /// Get whether fog is applied.
            /**
             * @return Whether fog is applied.
             */
            bool GetFogApply() const;

            /// Set fog density.
            /**
             * @param fogDensity Density of the fog.
             */
            void SetFogDensity(float fogDensity);

            /// get fog density.
            /**
             * @return Density of the fog
             */
            float GetFogDensity() const;

            /// Set fog color.
            /**
             * @param fogColor Color of the fog.
             */
            void SetFogColor(const glm::vec3& fogColor);

            /// Get fog color.
            /**
             * @return Color of the fog.
             */
            glm::vec3 GetFogColor() const;

            /// Set whether color filter is applied.
            /**
             * @param colorFilterApply Whether to apply color filter.
             */
            void SetColorFilterApply(bool colorFilterApply);

            /// Get whether color filter is applied.
            /**
             * @return Whether color filter is applied.
             */
            bool GetColorFilterApply() const;

            /// Set color filer color.
            /**
             * @param colorFilterColor Color of the color filter.
             */
            void SetColorFilterColor(const glm::vec3& colorFilterColor);

            /// Get color filer color.
            /**
             * @return Color of the color filter.
             */
            glm::vec3 GetColorFilterColor() const;

            /// Set whether dithering is applied.
            /**
             * @param ditherApply Whether to apply dithering.
             */
            void SetDitherApply(bool ditherApply);

            /// Get whether dithering is applied.
            /**
             * @return Whether dithering is applied.
             */
            bool GetDitherApply() const;

            /// Set size of the current frame to be rendered in pixels.
            /**
             * @param frameSize Size of the frame in pixels.
             */
            void SetFrameSize(const glm::vec2& frameSize);

            /// Get size of the current frame to be rendered in pixels.
            /**
             * @return Size of the frame in pixels.
             */
            glm::vec2 GetFrameSize() const;

        private:
            Renderer(const Renderer& other) = delete;
            StaticRenderProgram* staticRenderProgram;
            SkinRenderProgram* skinRenderProgram;

            unsigned int lightCount;
            StorageBuffer* lightBuffer;

            PostProcessing* postProcessing;
            FXAAFilter* fxaaFilter;

            // Icon rendering.
            ShaderProgram* iconShaderProgram;
            GLuint vertexBuffer;
            GLuint vertexArray;
            const Texture2D* currentIcon = nullptr;

            // Uniform locations.
            GLuint viewProjectionLocation;
            GLuint cameraPositionLocation;
            GLuint cameraUpLocation;
            GLuint baseImageLocation;
            GLuint positionLocation;

            Geometry::Rectangle* rectangle;
    };
}
