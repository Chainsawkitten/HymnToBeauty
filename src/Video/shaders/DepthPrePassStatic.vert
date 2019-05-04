/*
Vertex shader used for depth prepass.
*/
layout(location = 0) in vec3 vertexPosition;

MATRICES
{
    mat4 viewProjectionMatrix;
} matrices;

PUSH_CONSTANTS
{
    mat4 modelMatrix;
} pushConst;

void main () {
    vec4 worldPosition = pushConst.modelMatrix * vec4(vertexPosition, 1.0);
    gl_Position = matrices.viewProjectionMatrix * worldPosition;
    
    FixPosition();
}
