/*
Default 2D vertex shader.
*/
#version 400

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

uniform vec2 position;
uniform vec2 size;

out VertexData {
    vec2 texCoords;
} vertexOut;

void main () {
    vertexOut.texCoords = vec2(vertexTexture.x, 1.0 - vertexTexture.y);
    
    vec2 pos = (vertexPosition + vec2(1.0, 1.0)) * 0.5;
    vec2 bottomLeft = vec2(2.0 * position.x - 1.0, 1.0 - 2.0 * (position.y + size.y));
    pos = bottomLeft + 2.0 * size * pos;
    
    gl_Position = vec4(pos, 0.0, 1.0);
}