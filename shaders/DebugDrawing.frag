/*
Fragment shader used for debug drawing.
*/
#version 400

uniform vec3 color;

out vec4 fragmentColor;

void main() {
    fragmentColor = vec4(color, 1.0);
}
