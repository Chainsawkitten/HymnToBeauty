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

            Json::Value Save() const override;

            /// Particletype
            Video::ParticleSystemRenderer::EmitterSettings particleType;
    };
} // namespace Component
