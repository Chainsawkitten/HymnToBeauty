layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

PUSH_CONSTANTS
{
    uvec2 screenSize;
} pushConst;

STORAGE_BUFFER(0)
{
    vec4 color[];
} storageBuffer;

void main() {
    outColor = storageBuffer.color[int(gl_FragCoord.y) * pushConst.screenSize.x + int(gl_FragCoord.x)];
}
