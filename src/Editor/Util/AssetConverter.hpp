#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Engine/Geometry/AssetFileHandler.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Engine/Geometry/MathFunctions.hpp>

/// Convert 3D file to a .asset file.
/**
 * Uses assimp to convert a 3D file to a .asset file.
 * Following 3D formats are tested:
 * FBX (.fbx)
 */
class AssetConverter {
  public:
    /// Paths to materials.
    struct Material {
        /// Albedo texture (Color channel in Maya).
        std::string albedo;

        /// Normal map (Bump Mapping channel in Maya).
        std::string normal;

        /// Roughness (Specular color channel in Maya).
        std::string roughness;

        /// Metallic (Reflected color channel in Maya).
        std::string metallic;
    };

    /// Constructor.
    AssetConverter();

    /// Destructor.
    ~AssetConverter();

    /// Convert a mesh from fbx to .asset format.
    /**
     * @param filepath Filepath of the fbxfile.
     * @param destination Filepath of the destination inculding name and extension.
     * @param scale Rescale the model.
     * @param triangulate Should the mesh be triangulated?
     * @param importNormals Should normals be imported from the mesh?
     * @param importTangents Should tangents be imported from the mesh?
     * @param flipsUVs Flip UV/texture coordinates in y axis.
     * @param importMaterial Should materials be imported from the mesh?
     * @param materials Materials structure to store material paths in.
     * @param CPU Whether the mesh should be stored on the CPU.
     * @param GPU Whether the mesh should be stored on the GPU.
     */
    void Convert(const char* filepath, const char* destination, const glm::vec3& scale, bool triangulate, bool importNormals, bool importTangents, bool flipsUVs, bool importMaterial, Material& materials, bool CPU, bool GPU);

    /// Check after conversion if everything went well.
    /**
     * @return True if successfully converted model.
     */
    bool Success() const;

    /// String with errors and warnings.
    /**
     * @return A string with messages of what went wrong durning conversion.
     */
    std::string& GetErrorString();

  private:
    void ConvertMeshes(const aiScene* aScene, Geometry::AssetFileHandler* file, const glm::vec3& scale, bool flipUVs, bool CPU, bool GPU);
    void ConvertMesh(aiMesh* mesh, Geometry::AssetFileHandler* file, const glm::vec3& scale, bool flipUVs, bool CPU, bool GPU);
    Video::Geometry::VertexType::StaticVertex* ConvertStaticVertices(aiMesh* aMesh, Geometry::AssetFileHandler* file, unsigned int numVertices, const glm::vec3& scale, bool flipUVs);
    Video::Geometry::VertexType::SkinVertex* ConvertSkinnedVertices(aiMesh* aMesh, Geometry::AssetFileHandler* file, unsigned int numVertices, const glm::vec3& scale, bool flipUVs);
    void CalculateAABB(Geometry::MeshData* meshData, unsigned int numVertices);
    void LoadMaterial(aiMaterial* material, aiTextureType type, std::string& path);

    Assimp::Importer aImporter;

    bool success = true;
    std::string errorString;
};
