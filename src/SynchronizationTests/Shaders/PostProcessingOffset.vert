/*
Post-processing vertex shader which applies an offset from uniform buffer.
*/
layout(location = 0) out vec2 outTexCoords;

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

MATRICES
{
    vec2 offset;
} matrices;

void main () {
    if (VertexIndex < 3) {
        gl_Position = vec4(positions[VertexIndex] + matrices.offset, 0.0, 1.0);
        outTexCoords = uvs[VertexIndex];
    } else {
        gl_Position = vec4(20.0, 20.0, 1.0, 1.0);
        outTexCoords = vec2(0.0, 0.0);
    }

    FixPosition();
    FixFramebufferCoordinates(outTexCoords);
}
