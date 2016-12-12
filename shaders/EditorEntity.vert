/*
Vertex shader for editor entities.
*/
#version 400

layout(location = 0) in float padding;

uniform vec3 position;

void main () {
    gl_Position = vec4(position, 1.0);
}
