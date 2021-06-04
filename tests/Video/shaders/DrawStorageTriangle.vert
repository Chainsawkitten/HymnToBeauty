layout(location = 0) out vec3 color;

STORAGE_BUFFER(0)
{
    vec4 colors[3];
    vec2 positions[3];
} storageBuffer;

void main() {
    gl_Position = vec4(storageBuffer.positions[VertexIndex], 0.0, 1.0);
    color = storageBuffer.colors[VertexIndex].rgb;

    FixPosition();
}
