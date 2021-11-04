#pragma once

#include <string>
#include <vector>
#include <json/json.h>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <glm/glm.hpp>

class Entity;

/// Converts GLTF models to internal format.
/**
 * GLTF specification: https://github.com/KhronosGroup/glTF/
 */
class GLTFImporter {
  public:
    /// Import a GLTF model.
    /**
     * @param path The path to the model (not including filename).
     * @param filename The filename of the GLTF.
     */
    void Import(const std::string& path, const std::string& filename);

  private:
    struct Buffer {
        uint32_t length;
        char* data;
    };

    struct BufferView {
        uint32_t buffer;
        uint32_t offset;
        uint32_t length;
        /// @todo Target?
    };

    struct Accessor {
        uint32_t bufferView;
        uint32_t byteOffset;
        uint32_t count;
        enum ComponentType { BYTE = 5120, UNSIGNED_BYTE, SHORT, UNSIGNED_SHORT, UNSIGNED_INT = 5125, FLOAT } componentType;
        enum class Type { SCALAR, VEC2, VEC3, VEC4 } type; /// @todo Matrices.
        glm::vec3 min;
        glm::vec3 max;
    };

    struct Primitive {
        uint32_t vertexCount;
        uint32_t indexCount;
        Video::Geometry::VertexType::StaticVertex* vertices;
        uint32_t* indices;
        glm::vec3 min;
        glm::vec3 max;
        uint32_t material;
    };

    struct Texture {
        uint32_t image;
    };

    struct Material {
        uint32_t albedoTexture;
        uint32_t roughnessMetallicTexture;
        uint32_t normalTexture;
    };

    struct GLTF {
        Json::Value root;
        std::vector<Buffer> buffers;
        std::vector<BufferView> bufferViews;
        std::vector<Accessor> accessors;
        std::vector<std::string> meshes;
        std::vector<std::string> images;
        std::vector<Texture> textures;
        std::vector<Material> materials;
    };

    void ReadBuffers(const std::string& path, GLTF& gltf);
    void ReadBufferViews(GLTF& gltf);
    void ReadAccessors(GLTF& gltf);
    void ReadImages(const std::string& path, GLTF& gltf);
    void ReadTextures(GLTF& gltf);
    void ReadMaterials(GLTF& gltf);
    void LoadMeshes(const std::string& path, const std::string& filename, GLTF& gltf);
    Primitive LoadPrimitive(const GLTF& gltf, const Json::Value& primitive);
    void SavePrimitive(const Primitive& primitive, const std::string& filename);
    void LoadScenes(const std::string& path, const std::string& filename, GLTF& gltf);
    void LoadNode(GLTF& gltf, Entity* parent, uint32_t index);
};
