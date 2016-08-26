/*
Text rendering - Vertex Shader
*/
#version 400
layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

uniform vec2 position;
uniform vec2 size;

uniform vec2 uv1;
uniform vec2 uv2;

out VertexData {
    vec2 texCoords;
} vertexOut;

void main () {
    vertexOut.texCoords.x = uv1.x + vertexTexture.x * (uv2.x - uv1.x);
    vertexOut.texCoords.y = uv2.y + vertexTexture.y * (uv1.y - uv2.y);
    
    vec2 pos = (vertexPosition + vec2(1.0, 1.0)) * 0.5;
    vec2 bottomLeft = vec2(2.0 * position.x - 1.0, 1.0 - 2.0 * (position.y + size.y));
    pos = bottomLeft + 2.0 * size * pos;

    gl_Position = vec4(pos, 0.0, 1.0);
}
