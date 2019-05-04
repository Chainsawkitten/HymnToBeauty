/*
Skinning pass-through vertex shader - Vertex Shader
*/
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in uvec4 vertexBoneIDs;
layout(location = 5) in vec4 vertexWeights;

layout(location = 0) out VertexData {
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
} vertexOut;

const int MAX_BONES = 100;

MATRICES
{
    mat4 viewProjectionMatrix;
    mat4 viewMatrix;
    mat4 bones[MAX_BONES];
} matrices;

PUSH_CONSTANTS
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushConst;

void main () {
    vec4 position = (matrices.bones[vertexBoneIDs[0]] * vec4(vertexPosition, 1.0)) * vertexWeights[0];
    position += (matrices.bones[vertexBoneIDs[1]] * vec4(vertexPosition, 1.0)) * vertexWeights[1];
    position += (matrices.bones[vertexBoneIDs[2]] * vec4(vertexPosition, 1.0)) * vertexWeights[2];
    position += (matrices.bones[vertexBoneIDs[3]] * vec4(vertexPosition, 1.0)) * vertexWeights[3];
   
    vec4 normal = (matrices.bones[vertexBoneIDs[0]] * vec4(vertexNormal, 0.0)) * vertexWeights[0];
    normal += (matrices.bones[vertexBoneIDs[1]] * vec4(vertexNormal, 0.0)) * vertexWeights[1];
    normal += (matrices.bones[vertexBoneIDs[2]] * vec4(vertexNormal, 0.0)) * vertexWeights[2];
    normal += (matrices.bones[vertexBoneIDs[3]] * vec4(vertexNormal, 0.0)) * vertexWeights[3];
    
    vec4 worldPosition = pushConst.modelMatrix * vec4(position.xyz, 1.0);
    gl_Position = matrices.viewProjectionMatrix * worldPosition;
    vertexOut.pos = vec3(matrices.viewMatrix * worldPosition);
    vertexOut.normal = normalize(mat3(pushConst.normalMatrix) * normal.xyz);
    vertexOut.tangent = vertexTangent;
    vertexOut.texCoords = vertexTexture;
    
    FixPosition();
}
