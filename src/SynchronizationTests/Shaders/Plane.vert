/*
Simple pass-through vertex shader - Vertex Shader
*/
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoords;

MATRICES
{
    mat4 viewProjectionMatrix;
    mat4 viewMatrix;
    
} matrices;

struct PerInstance {
    mat4 modelMatrix;
    mat4 normalMatrix;
};

STORAGE_BUFFER(0)
{
    PerInstance data[];
} instances;

MATERIAL(0, mapAlbedo)
MATERIAL(1, mapOffset)

void main () {
    vec4 worldPosition = instances.data[InstanceIndex].modelMatrix * vec4(vertexPosition, 1.0);
    gl_Position = matrices.viewProjectionMatrix * (worldPosition + texture(mapOffset, vertexTexture).r * vec4(0.0, 1.0, 0.0, 0.0));
    outPosition = vec3(matrices.viewMatrix * worldPosition);
    outNormal = normalize(mat3(instances.data[InstanceIndex].normalMatrix) * vertexNormal);
    outTexCoords = vertexTexture;
    
    FixPosition();
}
