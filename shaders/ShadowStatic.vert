/*
Vertex shader used for shadowmapping.
*/
#version 400
layout(location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main () {
    gl_Position = lightSpaceMatrix * (model * vec4(vertexPosition, 1.0));
}
