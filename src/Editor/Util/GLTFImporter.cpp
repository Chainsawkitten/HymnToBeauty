#include "GLTFImporter.hpp"

#include <Engine/Hymn.hpp>
#include <Utility/Log.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Entity/World.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Material.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Model.hpp>
#include <fstream>
#include <assert.h>

void GLTFImporter::Import(const std::string& path, const std::string& filename) {
    const std::string fullPath = Hymn().GetPath() + "/" + path + "/";
    Log(Log::INFO) << "Importing " << fullPath + filename << "\n";

    GLTF gltf;

    // Parse json file.
    std::ifstream file(fullPath + filename);
    file >> gltf.root;
    file.close();

    // Buffers
    ReadBuffers(fullPath, gltf);
    Log(Log::INFO) << gltf.buffers.size() << " buffers read.\n";

    // Buffer views
    ReadBufferViews(gltf);
    Log(Log::INFO) << gltf.bufferViews.size() << " buffer views read.\n";

    // Accessors
    ReadAccessors(gltf);
    Log(Log::INFO) << gltf.accessors.size() << " accessors read.\n";

    /// @todo Samplers

    // Images
    ReadImages(path, gltf);

    // Textures
    ReadTextures(gltf);

    // Materials
    ReadMaterials(gltf);

    /// @todo Support animations.

    // Meshes
    LoadMeshes(path, filename, gltf);

    // Scenes
    LoadScenes(path, filename, gltf);

    // Cleanup.
    for (Buffer& buffer : gltf.buffers) {
        delete[] buffer.data;
    }
}

void GLTFImporter::ReadBuffers(const std::string& path, GLTF& gltf) {
    const Json::Value& buffersNode = gltf.root["buffers"];
    for (uint32_t i = 0; i < buffersNode.size(); ++i) {
        Buffer buffer;
        buffer.length = buffersNode[i]["byteLength"].asUInt();

        // Read buffer from binary file.
        buffer.data = new char[buffer.length];
        const std::string uri = path + FileSystem::DELIMITER + buffersNode[i]["uri"].asString();
        std::ifstream bufferFile;
        bufferFile.open(uri, std::ios::binary);
        bufferFile.read(buffer.data, buffer.length);
        bufferFile.close();

        gltf.buffers.push_back(buffer);
    }
}

void GLTFImporter::ReadBufferViews(GLTF& gltf) {
    const Json::Value& bufferViewsNode = gltf.root["bufferViews"];
    std::vector<BufferView> bufferViews;
    for (uint32_t i = 0; i < bufferViewsNode.size(); ++i) {
        BufferView bufferView;
        bufferView.buffer = bufferViewsNode[i]["buffer"].asUInt();
        bufferView.offset = bufferViewsNode[i]["byteOffset"].asUInt();
        bufferView.length = bufferViewsNode[i]["byteLength"].asUInt();

        gltf.bufferViews.push_back(bufferView);
    }
}

void GLTFImporter::ReadAccessors(GLTF& gltf) {
    const Json::Value& accessorsNode = gltf.root["accessors"];
    for (uint32_t i = 0; i < accessorsNode.size(); ++i) {
        Accessor accessor;
        accessor.bufferView = accessorsNode[i]["bufferView"].asUInt();
        accessor.byteOffset = accessorsNode[i]["byteOffset"].asUInt();
        accessor.count = accessorsNode[i]["count"].asUInt();
        accessor.componentType = static_cast<Accessor::ComponentType>(accessorsNode[i]["componentType"].asUInt());

        const std::string typeString = accessorsNode[i]["type"].asString();
        if (typeString == "SCALAR") {
            accessor.type = Accessor::Type::SCALAR;
        } else if (typeString == "VEC2") {
            accessor.type = Accessor::Type::VEC2;
        } else if (typeString == "VEC3") {
            accessor.type = Accessor::Type::VEC3;
        } else if (typeString == "VEC4") {
            accessor.type = Accessor::Type::VEC4;
        } else {
            Log(Log::ERR) << "Matrix types not supported.\n";
            assert(false);
        }

        if (accessorsNode[i]["sparse"] != Json::nullValue) {
            Log(Log::ERR) << "Sparse accessors not supported.\n";
            assert(false);
        }

        if (accessorsNode[i]["byteStride"] != Json::nullValue) {
            Log(Log::ERR) << "Byte stride not supported.\n";
            assert(false);
        }

        // We only care about min and max for positions.
        if (accessor.type == Accessor::Type::VEC3) {
            if (accessorsNode[i]["min"] != Json::nullValue) {
                accessor.min.x = accessorsNode[i]["min"][0].asFloat();
                accessor.min.y = accessorsNode[i]["min"][1].asFloat();
                accessor.min.z = accessorsNode[i]["min"][2].asFloat();
            }

            if (accessorsNode[i]["max"] != Json::nullValue) {
                accessor.max.x = accessorsNode[i]["max"][0].asFloat();
                accessor.max.y = accessorsNode[i]["max"][1].asFloat();
                accessor.max.z = accessorsNode[i]["max"][2].asFloat();
            }
        }

        gltf.accessors.push_back(accessor);
    }
}

void GLTFImporter::ReadImages(const std::string& path, GLTF& gltf) {
    const Json::Value& imagesNode = gltf.root["images"];
    for (uint32_t i = 0; i < imagesNode.size(); ++i) {
        const Json::Value& image = imagesNode[i];

        if (image["bufferView"] != Json::nullValue) {
            Log(Log::ERR) << "Image buffers not supported.\n";
            assert(false);
        }

        const std::string imageFile = path + "/" + image["uri"].asString();
        gltf.images.push_back(imageFile);
    }
}

void GLTFImporter::ReadTextures(GLTF& gltf) {
    Log(Log::WARNING) << "Samplers not supported.\n";

    const Json::Value& texturesNode = gltf.root["textures"];
    for (uint32_t i = 0; i < texturesNode.size(); ++i) {
        const Json::Value& textureNode = texturesNode[i];
        Texture texture;
        texture.image = textureNode["source"].asUInt();

        gltf.textures.push_back(texture);
    }
}

void GLTFImporter::ReadMaterials(GLTF& gltf) {
    const Json::Value& materialsNode = gltf.root["materials"];
    for (uint32_t i = 0; i < materialsNode.size(); ++i) {
        const Json::Value& materialNode = materialsNode[i];
        Material material;

        if (materialNode["alphaMode"] != Json::nullValue)
            Log(Log::WARNING) << "Alpha mode not supported.\n";
        
        if (materialNode["doubleSided"] != Json::nullValue)
            Log(Log::WARNING) << "Double sided geometry not supported.\n";

        if (materialNode["emissiveFactor"] != Json::nullValue)
            Log(Log::WARNING) << "Emissive factor not supported.\n";

        // Normal map.
        const Json::Value& normalNode = materialNode["normalTexture"];
        if (normalNode != Json::nullValue) {
            if (normalNode["texCoord"] != Json::nullValue)
                Log(Log::WARNING) << "Tex coord not supported.\n";

            if (normalNode["scale"] != Json::nullValue)
                Log(Log::WARNING) << "Normal map scale not supported.\n";

            material.normalTexture = normalNode["index"].asUInt();
        } else {
            material.normalTexture = ~0u;
        }

        // PBR textures.
        const Json::Value& pbrNode = materialNode["pbrMetallicRoughness"];
        if (pbrNode["baseColorFactor"] != Json::nullValue)
            Log(Log::WARNING) << "Base color factor not supported.\n";

        const Json::Value& baseColorNode = pbrNode["baseColorTexture"];
        if (baseColorNode != Json::nullValue) {
            if (baseColorNode["texCoord"] != Json::nullValue)
                Log(Log::WARNING) << "Tex coord not supported.\n";

            material.albedoTexture = baseColorNode["index"].asUInt();
        } else {
            material.albedoTexture = ~0u;
        }

        if (pbrNode["metallicFactor"] != Json::nullValue)
            Log(Log::WARNING) << "Metallic factor not supported.\n";

        if (pbrNode["roughnessFactor"] != Json::nullValue)
            Log(Log::WARNING) << "Roughness factor not supported.\n";

        const Json::Value& roughnessMetallicNode = pbrNode["metallicRoughnessTexture"];
        if (roughnessMetallicNode != Json::nullValue) {
            if (roughnessMetallicNode["texCoord"] != Json::nullValue)
                Log(Log::WARNING) << "Tex coord not supported.\n";

            material.roughnessMetallicTexture = roughnessMetallicNode["index"].asUInt();
        } else {
            material.roughnessMetallicTexture = ~0u;
        }


        gltf.materials.push_back(material);
    }
}

void GLTFImporter::LoadMeshes(const std::string& path, const std::string& filename, GLTF& gltf) {
    // Create folder for meshes.
    const std::string meshName = FileSystem::GetName(filename);
    const std::string folderName = Hymn().GetPath() + "/" + path + "/" + meshName;
    FileSystem::CreateDirectory(folderName.c_str());

    /// @todo UIDs for entities. Make sure they are actually unique.

    const Json::Value& meshesNode = gltf.root["meshes"];
    for (uint32_t i = 0; i < meshesNode.size(); ++i) {
        // Create scene.
        World* world = new World();
        world->CreateRoot();
        Entity* root = world->GetRoot();

        const Json::Value& primitivesNode = meshesNode[i]["primitives"];
        for (uint32_t j = 0; j < primitivesNode.size(); ++j) {
            // Load primitive.
            Primitive primitive = LoadPrimitive(gltf, primitivesNode[j]);

            // Save primitive as .mesh.
            const std::string primitiveName = path + "/" + meshName + "/Mesh" + std::to_string(i) + "Primitive" + std::to_string(j) + ".mesh";
            const std::string primitiveFilename = Hymn().GetPath() + "/" + primitiveName;
            SavePrimitive(primitive, primitiveFilename);

            // Cleanup.
            delete[] primitive.indices;
            delete[] primitive.vertices;

            // Add node to scene.
            Entity* entity = root->AddChild("Primitive" + std::to_string(j));

            Geometry::Model* model = Managers().resourceManager->CreateModel(primitiveName);
            Component::Mesh* meshComponent = entity->AddComponent<Component::Mesh>();
            meshComponent->model = model;

            Component::Material* materialComponent = entity->AddComponent<Component::Material>();
            const Material& material = gltf.materials[primitive.material];
            if (material.albedoTexture != ~0u) {
                materialComponent->albedo = Managers().resourceManager->CreateTextureAsset(gltf.images[gltf.textures[material.albedoTexture].image]);
            }

            if (material.roughnessMetallicTexture != ~0u) {
                materialComponent->roughnessMetallic = Managers().resourceManager->CreateTextureAsset(gltf.images[gltf.textures[material.roughnessMetallicTexture].image]);
            }

            if (material.normalTexture != ~0u) {
                materialComponent->normal = Managers().resourceManager->CreateTextureAsset(gltf.images[gltf.textures[material.normalTexture].image]);
            }
        }

        // Save scene.
        const std::string sceneName = path + "/" + meshName + "/" + "Mesh" + std::to_string(i) + ".scn";
        world->Save(Hymn().GetPath() + "/" + sceneName);
        gltf.meshes.push_back(sceneName);
        delete world;
    }
}

GLTFImporter::Primitive GLTFImporter::LoadPrimitive(const GLTF& gltf, const Json::Value& primitiveNode) {
    Primitive primitive;

    // Index data.
    const Accessor& indexAccessor = gltf.accessors[primitiveNode["indices"].asUInt()];
    primitive.indexCount = indexAccessor.count;
    primitive.indices = new uint32_t[primitive.indexCount];
    primitive.material = primitiveNode["material"].asUInt();
    assert(indexAccessor.type == Accessor::Type::SCALAR);
    const BufferView& bufferView = gltf.bufferViews[indexAccessor.bufferView];
    const Buffer& buffer = gltf.buffers[bufferView.buffer];

    uint32_t stride;
    switch (indexAccessor.componentType) {
    case Accessor::ComponentType::BYTE:
    case Accessor::ComponentType::UNSIGNED_BYTE:
        stride = sizeof(int8_t);
        break;
    case Accessor::ComponentType::SHORT:
    case Accessor::ComponentType::UNSIGNED_SHORT:
        stride = sizeof(uint16_t);
        break;
    case Accessor::ComponentType::UNSIGNED_INT:
        stride = sizeof(uint32_t);
        break;
    default:
        Log(Log::ERR) << "Indices have to be an integer type.\n";
        assert(false);
    }

    const char* indexValue = buffer.data + bufferView.offset;

    for (uint32_t i = 0; i < primitive.indexCount; ++i) {
        switch (indexAccessor.componentType) {
        case Accessor::ComponentType::BYTE:
            primitive.indices[i] = static_cast<uint32_t>(*reinterpret_cast<const int8_t*>(indexValue));
            break;
        case Accessor::ComponentType::UNSIGNED_BYTE:
            primitive.indices[i] = static_cast<uint32_t>(*reinterpret_cast<const uint8_t*>(indexValue));
            break;
        case Accessor::ComponentType::SHORT:
            primitive.indices[i] = static_cast<uint32_t>(*reinterpret_cast<const int16_t*>(indexValue));
            break;
        case Accessor::ComponentType::UNSIGNED_SHORT:
            primitive.indices[i] = static_cast<uint32_t>(*reinterpret_cast<const uint16_t*>(indexValue));
            break;
        case Accessor::ComponentType::UNSIGNED_INT:
            primitive.indices[i] = *reinterpret_cast<const int32_t*>(indexValue);
            break;
        }

        indexValue += stride;
    }

    // Vertex data.
    const Json::Value& attributesNode = primitiveNode["attributes"];

    // We're assuming all meshes have position, tex coord, normal and tangent.
    // Animated meshes are not currently supported.

    const Json::Value& positionNode = attributesNode["POSITION"];
    const Json::Value& texCoordNode = attributesNode["TEXCOORD_0"];
    const Json::Value& normalNode = attributesNode["NORMAL"];
    const Json::Value& tangentNode = attributesNode["TANGENT"];

    bool calculateNormals = false;
    bool calculateTangents = false;

    if (positionNode == Json::nullValue) {
        Log(Log::ERR) << "Position attribute is mandatory.\n";
        assert(false);
    }

    if (texCoordNode == Json::nullValue) {
        Log(Log::ERR) << "Tex coord attribute is mandatory.\n";
        assert(false);
    }

    if (normalNode == Json::nullValue) {
        /// @todo Calculate flat normals.
        // GLTF spec: When normals are not specified, client implementations should calculate flat normals.
        Log(Log::WARNING) << "Normal calculation not implemented.\n";
        calculateNormals = true;
    }

    if (tangentNode == Json::nullValue) {
        /// @todo Calculate tangents.
        // GLTF spec: When tangents are not specified, client implementations should calculate tangents using default MikkTSpace algorithms.
        // For best results, the mesh triangles should also be processed using default MikkTSpace algorithms.
        Log(Log::WARNING) << "Tangent calculation not implemented.\n";
        calculateTangents = true;
    }

    const Accessor& positionAccessor = gltf.accessors[positionNode.asUInt()];
    const Accessor& texCoordAccessor = gltf.accessors[texCoordNode.asUInt()];
    const Accessor& normalAccessor = gltf.accessors[normalNode.asUInt()];
    const Accessor& tangentAccessor = gltf.accessors[tangentNode.asUInt()];

    primitive.min = positionAccessor.min;
    primitive.max = positionAccessor.max;

    // Get vertex count.
    primitive.vertexCount = positionAccessor.count;
    if (primitive.vertexCount != texCoordAccessor.count || (!calculateNormals && primitive.vertexCount != normalAccessor.count) || (!calculateTangents && primitive.vertexCount != tangentAccessor.count)) {
        Log(Log::ERR) << "Not all accessors' counts are the same.\n";
    }
    primitive.vertices = new Video::Geometry::VertexType::StaticVertex[primitive.vertexCount];

    if (texCoordAccessor.componentType != Accessor::ComponentType::FLOAT) {
        /// @todo Support integer (normalized) texture coordinates.
        Log(Log::ERR) << "Integer texture coordinates not implemented.\n";
        assert(false);
    }

    // Get buffer views.
    const BufferView& positionBufferView = gltf.bufferViews[positionAccessor.bufferView];
    const BufferView& texCoordBufferView = gltf.bufferViews[texCoordAccessor.bufferView];
    const BufferView& normalBufferView = gltf.bufferViews[normalAccessor.bufferView];
    const BufferView* tangentBufferView = calculateTangents ? nullptr : &gltf.bufferViews[tangentAccessor.bufferView];

    // Get buffers.
    const Buffer& positionBuffer = gltf.buffers[positionBufferView.buffer];
    const Buffer& texCoordBuffer = gltf.buffers[texCoordBufferView.buffer];
    const Buffer& normalBuffer = gltf.buffers[normalBufferView.buffer];
    const Buffer* tangentBuffer = calculateTangents ? nullptr : &gltf.buffers[tangentBufferView->buffer];

    // Read vertex buffer data.
    const char* positionValue = positionBuffer.data + positionBufferView.offset + positionAccessor.byteOffset;
    const char* texCoordValue = texCoordBuffer.data + texCoordBufferView.offset + texCoordAccessor.byteOffset;
    const char* normalValue = normalBuffer.data + normalBufferView.offset + normalAccessor.byteOffset;
    const char* tangentValue = calculateTangents ? nullptr : (tangentBuffer->data + tangentBufferView->offset + tangentAccessor.byteOffset);

    for (uint32_t i = 0; i < primitive.vertexCount; ++i) {
        primitive.vertices[i].position.x = *reinterpret_cast<const float*>(positionValue);
        positionValue += sizeof(float);
        primitive.vertices[i].position.y = *reinterpret_cast<const float*>(positionValue);
        positionValue += sizeof(float);
        primitive.vertices[i].position.z = *reinterpret_cast<const float*>(positionValue);
        positionValue += sizeof(float);

        switch (texCoordAccessor.componentType) {
        case Accessor::ComponentType::UNSIGNED_BYTE:
        case Accessor::ComponentType::UNSIGNED_SHORT:
            /// @todo Integer texture coordinates.
            Log(Log::ERR) << texCoordAccessor.componentType << " Unsupported component type for tex coord.\n";
            primitive.vertices[i].textureCoordinate.x = 0.0f;
            primitive.vertices[i].textureCoordinate.y = 0.0f;
            break;
        case Accessor::ComponentType::FLOAT:
            primitive.vertices[i].textureCoordinate.x = *reinterpret_cast<const float*>(texCoordValue);
            texCoordValue += sizeof(float);
            primitive.vertices[i].textureCoordinate.y = *reinterpret_cast<const float*>(texCoordValue);
            texCoordValue += sizeof(float);
            break;
        default:
            Log(Log::ERR) << texCoordAccessor.componentType << " Invalid component type for tex coord.\n";
            assert(false);
        }

        primitive.vertices[i].normal.x = *reinterpret_cast<const float*>(normalValue);
        normalValue += sizeof(float);
        primitive.vertices[i].normal.y = *reinterpret_cast<const float*>(normalValue);
        normalValue += sizeof(float);
        primitive.vertices[i].normal.z = *reinterpret_cast<const float*>(normalValue);
        normalValue += sizeof(float);

        if (calculateTangents) {
            /// @todo
            primitive.vertices[i].tangent.x = 1.0f;
            primitive.vertices[i].tangent.y = 0.0f;
            primitive.vertices[i].tangent.z = 0.0f;
        } else {
            primitive.vertices[i].tangent.x = *reinterpret_cast<const float*>(tangentValue);
            tangentValue += sizeof(float);
            primitive.vertices[i].tangent.y = *reinterpret_cast<const float*>(tangentValue);
            tangentValue += sizeof(float);
            primitive.vertices[i].tangent.z = *reinterpret_cast<const float*>(tangentValue);
            tangentValue += sizeof(float);
        }
    }

    return primitive;
}

void GLTFImporter::SavePrimitive(const Primitive& primitive, const std::string& filename) {
    std::ofstream file;
    file.open(filename, std::ios::binary);

    file.write(reinterpret_cast<const char*>(&primitive.vertexCount), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&primitive.indexCount), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&primitive.min.x), sizeof(float));
    file.write(reinterpret_cast<const char*>(&primitive.min.y), sizeof(float));
    file.write(reinterpret_cast<const char*>(&primitive.min.z), sizeof(float));
    file.write(reinterpret_cast<const char*>(&primitive.max.x), sizeof(float));
    file.write(reinterpret_cast<const char*>(&primitive.max.y), sizeof(float));
    file.write(reinterpret_cast<const char*>(&primitive.max.z), sizeof(float));
    file.write(reinterpret_cast<const char*>(primitive.vertices), sizeof(Video::Geometry::VertexType::StaticVertex) * primitive.vertexCount);
    file.write(reinterpret_cast<const char*>(primitive.indices), sizeof(uint32_t) * primitive.indexCount);

    file.close();
}

void GLTFImporter::LoadScenes(const std::string& path, const std::string& filename, GLTF& gltf) {
    const Json::Value& scenes = gltf.root["scenes"];

    for (uint32_t i = 0; i < scenes.size(); ++i) {
        // Create scene.
        World* world = new World();
        world->CreateRoot();
        Entity* root = world->GetRoot();

        const Json::Value& scene = scenes[i];
        const Json::Value& sceneNodes = scene["nodes"];

        for (uint32_t j = 0; j < sceneNodes.size(); ++j) {
            LoadNode(gltf, root, sceneNodes[j].asUInt());
        }

        // Save scene.
        const std::string sceneName = (scene["name"] != Json::nullValue) ? scene["name"].asString() : FileSystem::GetName(filename) + std::to_string(i);
        world->Save(Hymn().GetPath() + "/" + path + "/" + sceneName + ".scn");
        delete world;
    }
}

void GLTFImporter::LoadNode(GLTF& gltf, Entity* parent, uint32_t index) {
    const Json::Value& node = gltf.root["nodes"][index];
    const std::string name = (node["name"] != Json::nullValue) ? node["name"].asString() : "Node";
    Entity* entity = parent->AddChild(name);

    // Translation.
    if (node["translation"] != Json::nullValue) {
        entity->SetPosition(glm::vec3(
            node["translation"][0].asFloat(),
            node["translation"][1].asFloat(),
            node["translation"][2].asFloat()
        ));
    }

    // Rotation.
    if (node["rotation"] != Json::nullValue) {
        entity->SetRotation(glm::quat(
            node["rotation"][3].asFloat(),
            node["rotation"][0].asFloat(),
            node["rotation"][1].asFloat(),
            node["rotation"][2].asFloat()
        ));
    }

    // Scale.
    if (node["scale"] != Json::nullValue) {
        entity->SetScale(glm::vec3(
            node["scale"][0].asFloat(),
            node["scale"][1].asFloat(),
            node["scale"][2].asFloat()
        ));
    }

    /// @todo Matrix.
    if (node["matrix"] != Json::nullValue)
        Log(Log::ERR) << "Matrix not supported.\n";

    // Mesh.
    if (node["mesh"] != Json::nullValue) {
        entity->InstantiateScene(gltf.meshes[node["mesh"].asUInt()]);
    }

    // Load children.
    const Json::Value& children = node["children"];
    if (children != Json::nullValue) {
        for (uint32_t i = 0; i < children.size(); ++i) {
            LoadNode(gltf, entity, children[i].asUInt());
        }
    }
}
