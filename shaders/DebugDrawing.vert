/*
Vertex shader used for debug drawing.
*/
#version 400
layout(location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 viewProjection;
uniform float size;

void main () {
    gl_Position = viewProjection * (model * vec4(vertexPosition, 1.0));
    gl_PointSize = size;
}
