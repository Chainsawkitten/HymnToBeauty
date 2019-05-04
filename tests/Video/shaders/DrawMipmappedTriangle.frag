layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)

void main() {
    outColor = textureLod(colorSampler, texCoords, 2.0);
}
