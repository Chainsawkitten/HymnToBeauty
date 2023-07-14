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

PUSH_CONSTANTS
{
    mat4 modelMatrix;
    mat4 normalMatrix;
} pushConstants;

void main () {
    vec4 worldPosition = pushConstants.modelMatrix * vec4(vertexPosition, 1.0);
    gl_Position = matrices.viewProjectionMatrix * worldPosition;
    outPosition = vec3(matrices.viewMatrix * worldPosition);
    outNormal = normalize(mat3(pushConstants.normalMatrix) * vertexNormal);
    outTexCoords = vertexTexture;
    
    FixPosition();
}
