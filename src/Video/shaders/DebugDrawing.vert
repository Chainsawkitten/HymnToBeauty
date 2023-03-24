/*
Vertex shader used for debug drawing.
*/
layout(location = 0) in vec3 vertexPosition;

layout(location = 0) out vec3 outColor;

MATRICES
{
    mat4 viewProjectionMatrix;
} matrices;

PUSH_CONSTANTS
{
    mat4 modelMatrix;
    vec4 colorSize;
} pushConst;

void main () {
    gl_Position = matrices.viewProjectionMatrix * (pushConst.modelMatrix * vec4(vertexPosition, 1.0));
    
    outColor = pushConst.colorSize.rgb;
    
    FixPosition();
}
