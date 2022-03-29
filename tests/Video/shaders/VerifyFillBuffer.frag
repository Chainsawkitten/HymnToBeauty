precision mediump int;
precision mediump float;

layout(location = 0) in highp vec2 texCoords;

layout(location = 0) out vec4 outColor;

// Read only SSBO.
// Each variable has to be on a new line.
STORAGE_BUFFER(0)
{
    uint mask;
} testBuffer;

// Push constants. (Uniforms in the OpenGL backend.)
// Each variable has to be on a new line.
PUSH_CONSTANTS
{
    uint lightCount;
} pushConst;

void main() {
    if (testBuffer.mask == pushConst.lightCount)
        outColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
        outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
