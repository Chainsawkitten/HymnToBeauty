#version 400

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

uniform vec2 topLeft;
uniform vec2 bottomRight;
uniform vec2 screenSize;

out VertexData {
	vec2 texCoords;
} vertexOut;

void main () {
    vertexOut.texCoords = vertexTexture;
    vec2 position = topLeft + (vertexPosition + 1.0)/2.0 * (bottomRight - topLeft);
    position = (position / screenSize) * 2.0 - 1.0;
    position.y = -position.y;
    gl_Position = vec4(position, 0.0, 1.0);
}