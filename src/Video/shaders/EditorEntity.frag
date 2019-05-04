/*
Textures editor entities.
*/
layout(location = 0) in VertexData {
    vec2 texCoords;
} vertexIn;

layout(location = 0) out vec4 outColor;

MATERIAL(0, mapColor)

void main () {
    outColor = texture(mapColor, vertexIn.texCoords);
}
