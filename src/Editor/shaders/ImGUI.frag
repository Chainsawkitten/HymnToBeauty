layout(location = 0) in vec2 Frag_UV;
layout(location = 1) in vec4 Frag_Color;

layout(location = 0) out vec4 outColor;

MATERIAL(0, mapColor)

void main() {
    outColor = Frag_Color * texture(mapColor, Frag_UV.st);
}
