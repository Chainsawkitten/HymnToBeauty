#pragma once

#include <map>
#include <GL/glew.h>

namespace Video {
    class TexturePNG;
    namespace Geometry {
        class Rectangle;
    }
}
namespace Geometry {
    class Cube;
    class Model;
}
namespace Audio {
    class SoundFile;
    class AudioMaterial;
}

namespace Animation {
    class AnimationClip;
    class AnimationController;
    class Skeleton;
}
class TextureAsset;
class ScriptFile;

/// Handles all resources.
class ResourceManager {
    friend class Hub;

    public:
        /// Constructor
        ResourceManager() {}

        /// Create an animation clip.
        /**
         * @param name Name of animation clip.
         * @return The animation clip instance.
         */
        Animation::AnimationClip* CreateAnimationClip(const std::string& name);

        /// Free the reference to the animation clip.
        /**
         * @param animationClip %Animation clip to dereference.
         */
        void FreeAnimationClip(Animation::AnimationClip* animationClip);

        /// Create an animation controller.
        /**
         * @param name Name of animation controller.
         * @return The animation controller instance.
         */
        Animation::AnimationController* CreateAnimationController(const std::string& name);

        /// Free the reference to the animation controller.
        /**
         * @param animationClip %Animation controller to dereference.
         */
        void FreeAnimationController(Animation::AnimationController* animationController);

        /// Create a skeleton.
        /**
         * @param name Name of skeleton.
         * @return The skeleton instance.
         */
        Animation::Skeleton* CreateSkeleton(const std::string& name);

        /// Free the reference to the skeleton.
        /**
         * @param skeleton %Skeleton to dereference.
         */
        void FreeSkeleton(Animation::Skeleton* skeleton);

        /// Create a model if it doesn't already exist.
        /**
         * @param name Name of model.
         * @return The model instance
         */
        Geometry::Model* CreateModel(const std::string& name);

        /// Free the reference to the model.
        /**
        * @param model %Model to dereference.
        */
        void FreeModel(Geometry::Model* model);

        /// Create a 2D texture if it doesn't already exist.
        /**
         * @param data Image file data.
         * @param dataLength Length of the image file data.
         * @return The %TexturePNG instance
         */
        Video::TexturePNG* CreateTexturePNG(const char* data, int dataLength);

        /// Free the reference to the 2D texture.
        /**
         * Deletes the instance if no more references exist.
         * @param texture %Texture to dereference.
         */
        void FreeTexturePNG(Video::TexturePNG* texture);

        /// Create a texture asset if it doesn't already exist.
        /**
         * @param name The name of the texture asset.
         * @return The %TextureAsset instance
         */
        TextureAsset* CreateTextureAsset(const std::string& name);

        /// Free the reference to the texture asset.
        /**
         * Deletes the instance if no more references exist.
         * @param textureAsset %TextureAsset to dereference.
         */
        void FreeTextureAsset(TextureAsset* textureAsset);

        /// Get the number of instances of a texture asset.
        /**
         * @param textureAsset The texture asset to check.
         * @return How many instances of the texture asset currently exist.
         */
        int GetTextureAssetInstanceCount(TextureAsset* textureAsset);

        /// Create a sound if it doesn't already exist.
        /**
         * @param name Name of the sound.
         * @return The %SoundBuffer instance.
         */
        Audio::SoundFile* CreateSound(const std::string& name);

        /// Free the reference to the sound.
        /**
         * Deletes the instance if no more references exist.
         * @param soundFile %SoundFile to dereference.
         */
        void FreeSound(Audio::SoundFile* soundFile);

        /// Create a script file if it doesn't already exist.
        /**
         * @param name Name of the script file.
         * @return The %ScriptFile instance.
         */
        ScriptFile* CreateScriptFile(const std::string& name);

        /// Free the reference to the script file.
        /**
         * Deletes the instance if no more references exist.
         * @param scriptFile %ScriptFile to dereference.
         */
        void FreeScriptFile(ScriptFile* scriptFile);

    private:
        ResourceManager(ResourceManager const&) = delete;
        void operator=(ResourceManager const&) = delete;

        // Rectangle
        Video::Geometry::Rectangle* rectangle = nullptr;
        int rectangleCount = 0;

        // Cube
        Geometry::Cube* cube = nullptr;

        int cubeCount = 0;

        // Model.
        struct ModelInstance {
            Geometry::Model* model;
            int count;
        };
        std::map<std::string, ModelInstance> models;
        std::map<Geometry::Model*, std::string> modelsInverse;

        // Animation clip.
        struct AnimationClipInstance {
            Animation::AnimationClip* animationClip;
            int count;
        };
        std::map<std::string, AnimationClipInstance> animationClips;
        std::map<Animation::AnimationClip*, std::string> animationClipsInverse;

        // Animation controller.
        struct AnimationControllerInstance {
            Animation::AnimationController* animationController;
            int count;
        };
        std::map<std::string, AnimationControllerInstance> animationControllers;
        std::map<Animation::AnimationController*, std::string> animationControllersInverse;

        // Skeleton.
        struct SkeletonInstance {
            Animation::Skeleton* skeleton;
            int count;
        };
        std::map<std::string, SkeletonInstance> skeletons;
        std::map<Animation::Skeleton*, std::string> skeletonsInverse;

        // TexturePNG.
        struct TexturePNGInstance {
            Video::TexturePNG* texture;
            int count;
        };
        std::map<const char*, TexturePNGInstance> textures;
        std::map<Video::TexturePNG*, const char*> texturesInverse;

        // Texture asset.
        struct TextureAssetInstance {
            TextureAsset* textureAsset;
            int count;
        };
        std::map<std::string, TextureAssetInstance> textureAssets;
        std::map<TextureAsset*, std::string> textureAssetsInverse;

        // Sound.
        struct SoundInstance {
            Audio::SoundFile* sound;
            int count;
        };
        std::map<std::string, SoundInstance> sounds;
        std::map<Audio::SoundFile*, std::string> soundsInverse;

        // ScriptFile.
        struct ScriptFileInstance {
            ScriptFile* scriptFile;
            int count;
        };
        std::map<std::string, ScriptFileInstance> scriptFiles;
        std::map<ScriptFile*, std::string> scriptFilesInverse;
};
