/*
Vertex shader used for debug drawing.
*/
layout(location = 0) out VertexData {
    vec3 color;
} vertexOut;

MATRICES
{
    mat4 viewProjectionMatrix;
} matrices;

PUSH_CONSTANTS
{
    vec4 position;
    vec4 colorSize;
    vec2 invScreenSize;
} pushConst;

const vec2 relativePosition[6] = vec2[](
    vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0),
    vec2(-1.0, 1.0), vec2(1.0, -1.0), vec2(1.0, 1.0)
);

void main () {
    gl_Position = matrices.viewProjectionMatrix * pushConst.position;
    
    vertexOut.color = pushConst.colorSize.rgb;
    
    FixPosition();
    
    gl_Position /= gl_Position.w;
    gl_Position.xy += relativePosition[VertexIndex] * pushConst.invScreenSize * pushConst.colorSize.w;
}
