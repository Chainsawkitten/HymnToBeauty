#pragma once

class Hub;
class Scene;

/// Handles rendering a scene.
class RenderManager {
    friend class Hub;
    
    public:
        /// Render Scene containing entities.
        /**
         * @param scene Contains a bunch of entities.
         */
        void Render(Scene& scene);
    
    private:
        RenderManager();
        ~RenderManager();
        RenderManager(RenderManager const&) = delete;
        void operator=(RenderManager const&) = delete;
};
