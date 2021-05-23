layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec3 color;

void main() {
    gl_Position = inPosition;
    color = inColor.rgb;

    FixPosition();
}
