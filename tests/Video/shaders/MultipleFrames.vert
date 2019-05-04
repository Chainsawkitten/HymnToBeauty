layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 texCoords;

MATRICES
{
    mat4 view;
} matrices;

PUSH_CONSTANTS
{
    mat4 modelMatrix;
} pushConst;

void main() {
    gl_Position = matrices.view * (pushConst.modelMatrix * vec4(inPosition, 1.0));
    texCoords = inTexCoords;

    FixPosition();
}
