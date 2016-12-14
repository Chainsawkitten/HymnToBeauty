#include "RenderManager.hpp"

#include <GL/glew.h>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "ParticleManager.hpp"
#include "DebugDrawingManager.hpp"
#include "Default3D.vert.hpp"
#include "Default3D.frag.hpp"
#include "Skinning.vert.hpp"
#include "EditorEntity.vert.hpp"
#include "EditorEntity.geom.hpp"
#include "EditorEntity.frag.hpp"
#include "Light.png.hpp"
#include "ParticleEmitter.png.hpp"
#include "SoundSource.png.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../RenderProgram/SkinRenderProgram.hpp"
#include "../RenderProgram/StaticRenderProgram.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Lens.hpp"
#include "../Component/Transform.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/Material.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Component/DirectionalLight.hpp"
#include "../Component/PointLight.hpp"
#include "../Component/SpotLight.hpp"
#include "../Component/SoundSource.hpp"
#include "../Geometry/Geometry3D.hpp"
#include "../Texture/Texture2D.hpp"
#include "../Lighting/DeferredLighting.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "../Physics/Frustum.hpp"
#include "../MainWindow.hpp"
#include "../RenderTarget.hpp"
#include "../PostProcessing/PostProcessing.hpp"
#include "../PostProcessing/FXAAFilter.hpp"
#include "../PostProcessing/GammaCorrectionFilter.hpp"
#include "../PostProcessing/GlowFilter.hpp"
#include "../PostProcessing/GlowBlurFilter.hpp"

using namespace Component;

RenderManager::RenderManager() {
    // Init shaders.
    defaultVertexShader = Managers().resourceManager->CreateShader(DEFAULT3D_VERT, DEFAULT3D_VERT_LENGTH, GL_VERTEX_SHADER);
    skinningVertexShader = Managers().resourceManager->CreateShader(SKINNING_VERT, SKINNING_VERT_LENGTH, GL_VERTEX_SHADER);
    defaultFragmentShader = Managers().resourceManager->CreateShader(DEFAULT3D_FRAG, DEFAULT3D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    staticShaderProgram = Managers().resourceManager->CreateShaderProgram({ defaultVertexShader, defaultFragmentShader });
    skinShaderProgram = Managers().resourceManager->CreateShaderProgram({ skinningVertexShader, defaultFragmentShader });
    staticRenderProgram = new StaticRenderProgram(staticShaderProgram);
    skinRenderProgram = new SkinRenderProgram(skinShaderProgram);
    
    editorEntityVertexShader = Managers().resourceManager->CreateShader(EDITORENTITY_VERT, EDITORENTITY_VERT_LENGTH, GL_VERTEX_SHADER);
    editorEntityGeometryShader = Managers().resourceManager->CreateShader(EDITORENTITY_GEOM, EDITORENTITY_GEOM_LENGTH, GL_GEOMETRY_SHADER);
    editorEntityFragmentShader = Managers().resourceManager->CreateShader(EDITORENTITY_FRAG, EDITORENTITY_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    editorEntityShaderProgram = Managers().resourceManager->CreateShaderProgram({ editorEntityVertexShader, editorEntityGeometryShader, editorEntityFragmentShader });
    
    // Init textures.
    particleEmitterTexture = Managers().resourceManager->CreateTexture2D(PARTICLEEMITTER_PNG, PARTICLEEMITTER_PNG_LENGTH);
    lightTexture = Managers().resourceManager->CreateTexture2D(LIGHT_PNG, LIGHT_PNG_LENGTH);
    soundSourceTexture = Managers().resourceManager->CreateTexture2D(SOUNDSOURCE_PNG, SOUNDSOURCE_PNG_LENGTH);
    
    deferredLighting = new DeferredLighting();
    
    // Init filters.
    postProcessing = new PostProcessing();
    fxaaFilter = new FXAAFilter();
    gammaCorrectionFilter = new GammaCorrectionFilter();
    glowFilter = new GlowFilter();
    glowBlurFilter = new GlowBlurFilter();
    
    // Create editor entity geometry.
    float vertex;
    
    glBindVertexArray(0);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(float), &vertex, GL_STATIC_DRAW);
    
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
    
    glBindVertexArray(0);
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeShader(defaultVertexShader);
    Managers().resourceManager->FreeShader(skinningVertexShader);
    Managers().resourceManager->FreeShader(defaultFragmentShader);
    Managers().resourceManager->FreeShaderProgram(staticShaderProgram);
    Managers().resourceManager->FreeShaderProgram(skinShaderProgram);
    delete staticRenderProgram;
    delete skinRenderProgram;
    
    Managers().resourceManager->FreeShader(editorEntityVertexShader);
    Managers().resourceManager->FreeShader(editorEntityGeometryShader);
    Managers().resourceManager->FreeShader(editorEntityFragmentShader);
    Managers().resourceManager->FreeShaderProgram(editorEntityShaderProgram);
    
    Managers().resourceManager->FreeTexture2D(particleEmitterTexture);
    Managers().resourceManager->FreeTexture2D(lightTexture);
    Managers().resourceManager->FreeTexture2D(soundSourceTexture);
    
    delete deferredLighting;
    
    delete postProcessing;
    delete fxaaFilter;
    delete gammaCorrectionFilter;
    delete glowFilter;
    delete glowBlurFilter;
    
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

void RenderManager::Render(Scene& scene) {
    // Find camera entity.
    Entity* camera = nullptr;
    std::vector<Lens*> lenses = scene.GetComponents<Lens>();
    for (Lens* lens : lenses) {
        if (lens->entity->GetComponent<Transform>() != nullptr)
            camera = lens->entity;
    };
    
    // Render from camera.
    if (camera != nullptr) {
        deferredLighting->SetTarget();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec2 screenSize(MainWindow::GetInstance()->GetSize());
        glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
        
        std::vector<Mesh*> meshes = scene.GetComponents<Mesh>();
        // Static render program.
        staticRenderProgram->PreRender(camera, screenSize);
        for (Mesh* mesh : meshes)
            if (mesh->geometry->GetType() == Geometry::Geometry3D::STATIC)
                staticRenderProgram->Render(mesh);
        staticRenderProgram->PostRender();

        // Skin render program.
        skinRenderProgram->PreRender(camera, screenSize);
        for (Mesh* mesh : meshes)
            if (mesh->geometry->GetType() == Geometry::Geometry3D::SKIN)
                skinRenderProgram->Render(mesh);
        skinRenderProgram->PostRender();
        
        // Light the scene.
        postProcessing->GetRenderTarget()->SetTarget();
        deferredLighting->Render(scene, camera);
        
        // Anti-aliasing.
        fxaaFilter->SetScreenSize(screenSize);
        postProcessing->ApplyFilter(fxaaFilter);
        
        // Render particles.
        Managers().particleManager->UpdateBuffer(scene);
        Managers().particleManager->Render(scene, camera);
        
        // Glow.
        glowBlurFilter->SetScreenSize(screenSize);
        int blurAmount = 1;
        for (int i = 0; i < blurAmount; ++i) {
            glowBlurFilter->SetHorizontal(true);
            postProcessing->ApplyFilter(glowBlurFilter);
            glowBlurFilter->SetHorizontal(false);
            postProcessing->ApplyFilter(glowBlurFilter);
        }
        postProcessing->ApplyFilter(glowFilter);
        
        // Gamma correction.
        postProcessing->ApplyFilter(gammaCorrectionFilter);
        
        // Render to back buffer.
        postProcessing->Render(true);
    }
}

void RenderManager::RenderEditorEntities(Scene& scene) {
    // Find camera entity.
    Entity* camera = nullptr;
    std::vector<Lens*> lenses = scene.GetComponents<Lens>();
    for (Lens* lens : lenses) {
        if (lens->entity->GetComponent<Transform>() != nullptr)
            camera = lens->entity;
    };
    
    // Render from camera.
    if (camera != nullptr) {
        editorEntityShaderProgram->Use();
        glBindVertexArray(vertexArray);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        
        // Set camera uniforms.
        glm::vec2 screenSize(MainWindow::GetInstance()->GetSize());
        Transform* cameraTransform = camera->GetComponent<Component::Transform>();
        glm::mat4 viewMat(cameraTransform->GetCameraOrientation() * glm::translate(glm::mat4(), -cameraTransform->position));
        glm::mat4 projectionMat(camera->GetComponent<Lens>()->GetProjection(screenSize));
        glm::mat4 viewProjectionMat(projectionMat * viewMat);
        glm::vec3 up(glm::inverse(cameraTransform->GetCameraOrientation())* glm::vec4(0, 1, 0, 1));
    
        glUniformMatrix4fv(editorEntityShaderProgram->GetUniformLocation("viewProjectionMatrix"), 1, GL_FALSE, &viewProjectionMat[0][0]);
        glUniform3fv(editorEntityShaderProgram->GetUniformLocation("cameraPosition"), 1, &cameraTransform->position[0]);
        glUniform3fv(editorEntityShaderProgram->GetUniformLocation("cameraUp"), 1, &up[0]);
        glUniform1i(editorEntityShaderProgram->GetUniformLocation("baseImage"), 0);
        
        // Render sound sources.
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, soundSourceTexture->GetTextureID());
        
        for (SoundSource* soundSource : scene.GetComponents<SoundSource>())
            RenderEditorEntity(soundSource);
        
        // Render particle emitters.
        glBindTexture(GL_TEXTURE_2D, particleEmitterTexture->GetTextureID());
        
        for (ParticleEmitter* emitter : scene.GetComponents<ParticleEmitter>())
            RenderEditorEntity(emitter);
        
        // Render light sources.
        glBindTexture(GL_TEXTURE_2D, lightTexture->GetTextureID());
        
        for (DirectionalLight* light : scene.GetComponents<DirectionalLight>())
            RenderEditorEntity(light);
        
        for (PointLight* light : scene.GetComponents<PointLight>())
            RenderEditorEntity(light);
        
        for (SpotLight* light : scene.GetComponents<SpotLight>())
            RenderEditorEntity(light);
        
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}

void RenderManager::RenderEditorEntity(SuperComponent* component) {
    Entity* entity = component->entity;
    Transform* transform = entity->GetComponent<Transform>();
    if (transform != nullptr) {
        glUniform3fv(editorEntityShaderProgram->GetUniformLocation("position"), 1, &transform->position[0]);
        glDrawArrays(GL_POINTS, 0, 1);
    }
}
