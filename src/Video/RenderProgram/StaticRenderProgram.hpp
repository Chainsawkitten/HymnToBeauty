#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "RenderProgram.hpp"

namespace Video {
    class Texture2D;
    class ShaderProgram;
    class StorageBuffer;
    namespace Geometry {
        class Geometry3D;
    }

    /// Render program to render an entity using default shader program.
    class StaticRenderProgram : public RenderProgram {
        public:
            /// Create new static render program.
            StaticRenderProgram();

            /// Destructor.
            ~StaticRenderProgram();

            /// Bind shadow render program.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param shadowId The id for the shadowtexture.
             * @param shadowMapSize The size of the shadowmap.
             * @param depthFbo The framebufferobject containing the shadowmap.
             */
            void PreShadowRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, int shadowId, unsigned int shadowMapSize, int depthFbo);

            /// Render shadow pass.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The lights's view matrix.
             * @param projectionMatrix The lights's projection matrix.
             * @param modelMatrix Model matrix.
             */
            void ShadowRender(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) const;

            /// Bind depth render program.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             */
            void PreDepthRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

            /// Render depth pass.
            /**
             * @param geometry The geometry to render.
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param modelMatrix Model matrix.
             */
            void DepthRender(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) const;

            /// Bind render program.
            /**
             * @param viewMatrix The camera's view matrix.
             * @param projectionMatrix The camera's projection matrix.
             * @param lightBuffer %StorageBuffer containing light data.
             * @param lightCount Number of lights in the light buffer.
             * @param cameraNear Camera near plane distance.
             * @param cameraFar Camera far plane distance.
             */
            void PreRender(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const StorageBuffer* lightBuffer, unsigned int lightCount, float cameraNear, float cameraFar);

            /// Render mesh.
            /**
             * @param geometry The geometry to render.
             * @param textureAlbedo Albedo texture.
             * @param normalTexture Normal texture.
             * @param textureMetallic Metallic texture.
             * @param textureRoughness Roughness texture.
             * @param modelMatrix Model matrix.
             */
            void Render(Geometry::Geometry3D* geometry, const Video::Texture2D* textureAlbedo, const Video::Texture2D* normalTexture, const Video::Texture2D* textureMetallic, const Video::Texture2D* textureRoughness, const glm::mat4& modelMatrix) const;

        private:
            StaticRenderProgram(const StaticRenderProgram& other) = delete;
            ShaderProgram* shadowProgram;
            ShaderProgram* zShaderProgram;
            ShaderProgram* shaderProgram;

            // Uniform locations.
            GLuint shadowLightSpaceLocation;
            GLuint shadowModelLocation;
            GLuint zViewProjectionLocation;
            GLuint zModelLocation;
            GLuint viewProjectionLocation;
            GLuint lightSpaceLocation;
            GLuint lightCountLocation;
            GLuint cameraNearPlaneLocation;
            GLuint cameraFarPlaneLocation;
            GLuint gammaLocation;
            GLuint fogApplyLocation;
            GLuint fogDensityLocation;
            GLuint fogColorLocation;
            GLuint colorFilterApplyLocation;
            GLuint colorFilterColorLocation;
            GLuint ditherApplyLocation;
            GLuint timeLocation;
            GLuint frameSizeLocation;
            GLuint mapAlbedoLocation;
            GLuint mapNormalLocation;
            GLuint mapMetallicLocation;
            GLuint mapRoughnessLocation;
            GLuint mapShadowLocation;
            GLuint modelLocation;
            GLuint viewLocation;
            GLuint normalLocation;

            bool first = true;

            glm::mat4 viewMatrix;
            glm::mat4 projectionMatrix;
            glm::mat4 viewProjectionMatrix;
            glm::mat4 lightSpaceMatrix;

            int shadowId = 0;
    };
} // namespace Video
