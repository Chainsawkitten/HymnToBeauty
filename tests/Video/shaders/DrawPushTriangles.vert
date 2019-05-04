layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outColor;

PUSH_CONSTANTS
{
    mat4 modelMatrix;
    vec3 color;
} pushConst;

void main() {
    gl_Position = pushConst.modelMatrix * vec4(inPosition, 1.0);
    outColor = pushConst.color;

    FixPosition();
}
