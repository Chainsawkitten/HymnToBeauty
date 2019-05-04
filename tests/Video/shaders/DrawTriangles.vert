layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 color;

MATRICES
{
    mat4 model;
} matrices;

void main() {
    gl_Position = matrices.model * vec4(inPosition, 1.0);
    color = inColor;

    FixPosition();
}
