layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)

void main() {
    outColor = vec4(vec3(1.0, 1.0, 1.0) - texture(colorSampler, texCoords).rgb, 1.0);
}
