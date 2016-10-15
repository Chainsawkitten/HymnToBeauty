/*
Vertex shader used for debug drawing.
*/
#version 400
layout(location = 0) in vec3 vertexPosition;

uniform mat4 viewProjection;

void main () {
    gl_Position = viewProjection * vec4(vertexPosition, 1.0);
}
