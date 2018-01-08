#pragma once

#include <Engine/Component/SuperComponent.hpp>
#include <glm/glm.hpp>
#include <Video/ParticleSystemRenderer.hpp>

namespace Component {
    /// Emitter that emits particles.
    class ParticleSystemComponent : public SuperComponent {
        public:
            /// Create new particle emitter.
            ParticleSystemComponent();

            /// Destructor.
            ~ParticleSystemComponent();

            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;

            /// Particletype
            Video::ParticleSystemRenderer::EmitterSettings particleType;
    };
} // namespace Component
