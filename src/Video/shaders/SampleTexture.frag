layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)

void main() {
    outColor = texture(colorSampler, inTexCoords);
}
