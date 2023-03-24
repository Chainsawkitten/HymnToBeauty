/*
Simple pass-through vertex shader - Vertex Shader
*/
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outTangent;
layout(location = 3) out vec2 outTexCoords;

MATRICES
{
    mat4 viewProjectionMatrix;
    mat4 viewMatrix;
} matrices;

PUSH_CONSTANTS
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushConst;

void main () {
    vec4 worldPosition = pushConst.modelMatrix * vec4(vertexPosition, 1.0);
    gl_Position = matrices.viewProjectionMatrix * worldPosition;
    outPosition = vec3(matrices.viewMatrix * worldPosition);
    outNormal = normalize(mat3(pushConst.normalMatrix) * vertexNormal);
    outTangent = vertexTangent;
    outTexCoords = vertexTexture;
    
    FixPosition();
}
