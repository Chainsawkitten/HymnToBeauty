layout(location = 0) out vec2 texCoords;

const vec2 positions[3] = vec2[](
    vec2(-1.0, 3.0),
    vec2(-1.0, -1.0),
    vec2(3.0, -1.0)
);

const vec2 uvs[3] = vec2[](
    vec2(0.0, -1.0),
    vec2(0.0, 1.0),
    vec2(2.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[VertexIndex], 0.0, 1.0);
    texCoords = uvs[VertexIndex];

    // These functions handle OpenGL-Vulkan differences in clip space and texture coordinates (when reading back render targets).
    // Not relevant for the reproducer test. The texture coordinates aren't used and the fullscreen triangle is a single color so doesn't matter if it's upside down.
    FixPosition();
    FixFramebufferCoordinates(texCoords);
}
