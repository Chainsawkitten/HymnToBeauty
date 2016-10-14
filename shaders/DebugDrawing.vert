/*
Vertex shader used for debug drawing.
*/
#version 400
layout(location = 0) in vec3 vertexPosition;

void main () {
    gl_Position = vec4(vertexPosition, 1.0);
}
