/*
Fragment shader used for debug drawing.
*/
layout(location = 0) in VertexData {
    vec3 color;
} vertexIn;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(vertexIn.color, 1.0);
}
