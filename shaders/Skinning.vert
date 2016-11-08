/*
Skinning pass-through vertex shader - Vertex Shader
*/
#version 400
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in ivec4 vertexBoneIDs;
layout(location = 5) in vec4 vertexWeights;

const int MAX_BONES = 100;

uniform mat4 viewProjection;
uniform mat4 model;
uniform mat3 normalMatrix;
uniform mat4 bones[MAX_BONES];
uniform mat3 bonesIT[MAX_BONES];

out VertexData {
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
} vertexOut;

void main () {
    vec4 position = (bones[vertexBoneIDs[0]] * vec4(vertexPosition, 1.0)) * vertexWeights[0];
    position += (bones[vertexBoneIDs[1]] * vec4(vertexPosition, 1.0)) * vertexWeights[1];
    position += (bones[vertexBoneIDs[2]] * vec4(vertexPosition, 1.0)) * vertexWeights[2];
    position += (bones[vertexBoneIDs[3]] * vec4(vertexPosition, 1.0)) * vertexWeights[3];
    
    vec3 normal = (bonesIT[vertexBoneIDs[0]] * vertexNormal) * vertexWeights[0];
    normal += (bonesIT[vertexBoneIDs[1]] * vertexNormal) * vertexWeights[1];
    normal += (bonesIT[vertexBoneIDs[2]] * vertexNormal) * vertexWeights[2];
    normal += (bonesIT[vertexBoneIDs[3]] * vertexNormal) * vertexWeights[3];
    
    gl_Position = viewProjection * (model * position);
    vertexOut.normal = normalize(normalMatrix * normal);
    vertexOut.tangent = vertexTangent;
    vertexOut.texCoords = vertexTexture;
}
