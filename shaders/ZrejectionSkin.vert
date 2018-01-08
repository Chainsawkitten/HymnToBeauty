/*
Vertex shader used for Early Z forward rendering.
*/
#version 400
layout(location = 0) in vec3 vertexPosition;
layout(location = 4) in ivec4 vertexBoneIDs;
layout(location = 5) in vec4 vertexWeights;

const int MAX_BONES = 100;

uniform mat4 model;
uniform mat4 viewProjection;
uniform mat4 bones[MAX_BONES];

void main () {
    vec4 position = (bones[vertexBoneIDs[0]] * vec4(vertexPosition, 1.0)) * vertexWeights[0];
    position += (bones[vertexBoneIDs[1]] * vec4(vertexPosition, 1.0)) * vertexWeights[1];
    position += (bones[vertexBoneIDs[2]] * vec4(vertexPosition, 1.0)) * vertexWeights[2];
    position += (bones[vertexBoneIDs[3]] * vec4(vertexPosition, 1.0)) * vertexWeights[3];
    
    gl_Position = viewProjection * (model * vec4(position.xyz, 1.0));
}
