#version 400

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

uniform vec2 position;
uniform vec2 size;
uniform vec2 screenSize;

out VertexData {
	vec2 texCoords;
} vertexOut;

void main () {
    vertexOut.texCoords = vec2(vertexTexture.x, 1.0 - vertexTexture.y);
    
    vec2 pos = (vertexPosition + vec2(1.0, 1.0)) / 2.0;
	pos = position + size * pos;
	pos = 2.0 * pos - vec2(1.0, 1.0);
	pos.y = -pos.y;
    
    gl_Position = vec4(pos, 0.0, 1.0);
}