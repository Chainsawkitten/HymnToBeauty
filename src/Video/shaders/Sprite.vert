/*
Vertex shader for sprites.
*/

layout(location = 0) in vec2 vertexTexture;

layout(location = 0) out vec2 outTexCoords;
layout(location = 1) out flat mediump vec4 outTint;

MATRICES
{
    mat4 viewProjectionMatrix;
} matrices;

PUSH_CONSTANTS
{
    mat4 modelMatrix;
    vec4 sizePivot;
    vec4 tint;
} pushConst;

void main () {
    outTexCoords = vertexTexture;
    outTexCoords.y = 1.0 - outTexCoords.y;
    outTint = pushConst.tint;
    vec2 position = (vertexTexture - pushConst.sizePivot.zw) * pushConst.sizePivot.xy;
    
    gl_Position = matrices.viewProjectionMatrix * (pushConst.modelMatrix * vec4(position, 0.0, 1.0));
    
    FixPosition();
}
