/*
Post-processing vertex shader.
*/
#version 400

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

out VertexData {
    vec2 texCoords;
} vertexOut;

void main () {
    vertexOut.texCoords = vertexTexture;
    gl_Position = vec4(vertexPosition, 0.0, 1.0);
}
