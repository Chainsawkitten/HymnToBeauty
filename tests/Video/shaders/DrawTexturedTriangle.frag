layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)
MATERIAL(1, alphaSampler)

void main() {
    outColor = vec4(texture(colorSampler, texCoords).rgb, texture(alphaSampler, texCoords).a);
}
