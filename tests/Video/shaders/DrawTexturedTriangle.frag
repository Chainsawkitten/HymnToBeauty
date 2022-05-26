layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorTexture)
MATERIAL(1, alphaTexture)

void main() {
    outColor = vec4(texture(colorTexture, texCoords).rgb, texture(alphaTexture, texCoords).a);
}
