/*
Render each fragment as a single color.
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform vec3 color;

out vec4 fragmentColor;

void main () {
    fragmentColor = vec4(color, 1.0);
}