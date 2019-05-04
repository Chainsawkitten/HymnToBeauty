/*
Vertex shader for editor entities.
*/

layout(location = 0) in vec2 vertexTexture;

layout(location = 0) out VertexData {
    vec2 texCoords;
} vertexOut;

MATRICES
{
    mat4 viewProjectionMatrix;
} matrices;

PUSH_CONSTANTS
{
    vec4 position;
    vec4 right;
    vec4 up;
} pushConst;

void main () {
    vec3 position = pushConst.position.xyz + pushConst.right.xyz * (vertexTexture.x - 0.5) - pushConst.up.xyz * (vertexTexture.y - 0.5);
    vertexOut.texCoords = vertexTexture;
    
    gl_Position = matrices.viewProjectionMatrix * vec4(position, 1.0);
    
    FixPosition();
}
