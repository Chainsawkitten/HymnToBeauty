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
	//vertexOut.tex_coords = vertex_position;
	vertexOut.texCoords.x = uv1.x + vertexTexture.x * (uv2.x - uv1.x);
	vertexOut.texCoords.y = uv1.y + vertexTexture.y * (uv2.y - uv1.y);
	
    vec2 pos = (vertexPosition + vec2(1.0, 1.0)) / 2.0;
	pos = position + size * pos;
	pos = 2.0 * pos - vec2(1.0, 1.0);
	pos.y = -pos.y;

	gl_Position = vec4(pos, 0.0, 1.0);
}