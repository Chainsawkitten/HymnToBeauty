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

out VertexData {
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
} vertexOut;

void main () {
    mat4 boneTransform = bones[vertexBoneIDs[0]] * vertexWeights[0];
    boneTransform += bones[vertexBoneIDs[1]] * vertexWeights[1];
    boneTransform += bones[vertexBoneIDs[2]] * vertexWeights[2];
    boneTransform += bones[vertexBoneIDs[3]] * vertexWeights[3];
    
    gl_Position = viewProjection * (model * (boneTransform * vec4(vertexPosition, 1.0)));
    vertexOut.normal = normalize(normalMatrix * vec3(boneTransform * vec4(vertexNormal, 0.0)));
    vertexOut.tangent = vertexTangent;
    vertexOut.texCoords = vertexTexture;
}
