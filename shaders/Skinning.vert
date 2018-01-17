/*
Skinning pass-through vertex shader - Vertex Shader
*/
#version 430
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
uniform mat4 viewMatrix;
uniform mat4 bones[MAX_BONES];
uniform mat4 lightSpaceMatrix;

out VertexData {
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} vertexOut;

void main () {
    vec4 position = (bones[vertexBoneIDs[0]] * vec4(vertexPosition, 1.0)) * vertexWeights[0];
    position += (bones[vertexBoneIDs[1]] * vec4(vertexPosition, 1.0)) * vertexWeights[1];
    position += (bones[vertexBoneIDs[2]] * vec4(vertexPosition, 1.0)) * vertexWeights[2];
    position += (bones[vertexBoneIDs[3]] * vec4(vertexPosition, 1.0)) * vertexWeights[3];
   
    vec4 normal = (bones[vertexBoneIDs[0]] * vec4(vertexNormal, 0.0)) * vertexWeights[0];
    normal += (bones[vertexBoneIDs[1]] * vec4(vertexNormal, 0.0)) * vertexWeights[1];
    normal += (bones[vertexBoneIDs[2]] * vec4(vertexNormal, 0.0)) * vertexWeights[2];
    normal += (bones[vertexBoneIDs[3]] * vec4(vertexNormal, 0.0)) * vertexWeights[3];
    
    vec4 worldPosition = model * vec4(position.xyz, 1.0);
    gl_Position = viewProjection * worldPosition;
    vertexOut.pos = vec3(viewMatrix * worldPosition);
    vertexOut.normal = normalize(normalMatrix * normal.xyz);
    vertexOut.tangent = vertexTangent;
    vertexOut.texCoords = vertexTexture;
    vertexOut.fragPosLightSpace = lightSpaceMatrix * worldPosition;
}
