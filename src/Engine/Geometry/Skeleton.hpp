#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>

struct aiScene;
struct aiNode;
class Animation;

/// A skeleton loaded from a file.
class Skeleton {
    public:
        /// Create new empty skeleton.
        /**
         * The created skeleton has to be loaded later using Load.
         */
        Skeleton();

        /// Create new skeleton.
        /**
         * @param aScene Pointer to assimp scene.
         */
        Skeleton(const aiScene* aScene);

        /// Destructor.
        ~Skeleton();

        /// Load skeleton.
        /**
         * @param aScene Pointer to assimp scene.
         */
        void Load(const aiScene* aScene);

        /// Get bone index.
        /**
         * @param name Bone name.
         * @return Bone index, if not found -1.
         */
        std::size_t FindBoneIndex(const std::string& name);

        /// Get number of bones.
        /**
         * @return Number of bones.
         */
        std::size_t GetNumBones() const;

        /// Animate skeleton.
        /**
         * @param animation Animation to animate skeleton.
         * @param timeInSeconds Time to find animation frame.
         * @param transforms Vector of matrices to store bone transformation after animation.
         */
        void Animate(const Animation* animation, const float timeInSeconds);

        /// Get vector of transformations of skeleton.
        /**
        * Final transformations are updated when skeleton is animated.
        * @retrun animation Animation to animate skeleton.
        */
        const std::vector<glm::mat4>& GetFinalTransformations() const;

    private:
        struct Node {
            std::string name;
            glm::mat4 transformation;
            Node* parent;
            std::vector<Node> children;
        };

        static void LoadNodeTree(aiNode* aNode, Node* node, Node* parentNode);
        void ReadNodeHeirarchy(const Animation* animation, float animationTime, Node* node, const glm::mat4& parentTransform);
        const glm::mat4* FindBone(const std::string& name) const;

        glm::mat4 globalInverseTransform;
        Node rootNode;
        std::vector<glm::mat4> bones;
        std::vector<glm::mat4> finalTransforms;
        std::map<std::string, std::size_t> boneIndexMap;
};
