layout(location = 0) in vec2 texCoords;

PUSH_CONSTANTS
{
    uvec2 screenSize;
} pushConst;

STORAGE_BUFFER_RW(0)
{
    vec4 color[];
} storageBuffer;

void main() {
    storageBuffer.color[int(gl_FragCoord.y) * pushConst.screenSize.x + int(gl_FragCoord.x)] = vec4(texCoords.x, 1.0, 0.0, 1.0);
}
