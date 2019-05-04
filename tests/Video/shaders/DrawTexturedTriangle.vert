layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoords;

layout(location = 0) out vec2 texCoords;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    texCoords = inTexCoords;

    FixPosition();
}
