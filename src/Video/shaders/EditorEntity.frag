/*
Textures editor entities.
*/
layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, mapColor)

void main () {
    outColor = texture(mapColor, texCoords);
}
