/*
Vertex shader used for depth prepass.
*/
layout(location = 0) in vec3 vertexPosition;
layout(location = 4) in uvec4 vertexBoneIDs;
layout(location = 5) in vec4 vertexWeights;

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
} pushConst;

void main () {
    vec4 position = (matrices.bones[vertexBoneIDs[0]] * vec4(vertexPosition, 1.0)) * vertexWeights[0];
    position += (matrices.bones[vertexBoneIDs[1]] * vec4(vertexPosition, 1.0)) * vertexWeights[1];
    position += (matrices.bones[vertexBoneIDs[2]] * vec4(vertexPosition, 1.0)) * vertexWeights[2];
    position += (matrices.bones[vertexBoneIDs[3]] * vec4(vertexPosition, 1.0)) * vertexWeights[3];
    
    gl_Position = matrices.viewProjectionMatrix * (pushConst.modelMatrix * vec4(position.xyz, 1.0));
    
    FixPosition();
}
