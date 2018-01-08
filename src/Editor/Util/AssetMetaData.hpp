#pragma once

#include <ctime>
#include <json/json.h>

/// Static class for metadata generation.
/**
 * A metadata file contains information about a file.
 * The extension for a meta file is .meta.
 * Current metadata support exists for:
 * Meshes
 * 
 * @todo To be implemented:
 * Materials
 * Animations
 * Skeletons
 */
class AssetMetaData
{
    public:
        /// Metadata for mesh asset.
        struct MeshMetaData {
            bool triangulate;
            bool importNormals;
            bool importTangents;
            bool CPU;
            bool GPU;
        };

        /// Save metadata for a mesh.
        /**
         * @param filepath Filepath with extensions.
         * @param metaData Metadata to be exported.
         * @return Whether the file could be opened.
         */
        static bool SaveMetaData(const char* filepath, MeshMetaData* metaData);

        /// Load metadata for a mesh.
        /**
         * @param filepath Filepath with extensions.
         * @param metaData Metadata to be imported.
         * @return Whether the file could be opened.
         */
        static bool LoadMetaData(const char* filepath, MeshMetaData* metaData);

    private:
        /// Constructor.
        AssetMetaData() {}

        /// Destructor.
        ~AssetMetaData() {}

        static void ToJson(const MeshMetaData* metaData, Json::Value& node);

        static void FromJson(const Json::Value& node, MeshMetaData* metaData);

        static bool Save(const char* filepath, Json::Value& node);

        static bool Load(const char* filepath, Json::Value& node);
};
