layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

STORAGE_BUFFER(0)
{
    vec4 color;
} storageBuffer;

void main() {
    outColor = storageBuffer.color;
}
