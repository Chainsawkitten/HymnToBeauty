/*
Vertex shader used for Early Z forward rendering.
*/
#version 400
layout(location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 viewProjection;

void main () {
    gl_Position = viewProjection * (model * vec4(vertexPosition, 1.0));
}
