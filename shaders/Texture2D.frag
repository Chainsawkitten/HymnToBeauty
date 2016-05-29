/*
Render a texture to the screen.
*/
#version 400

in VertexData {
	vec2 texCoords;
} vertexIn;

uniform sampler2D baseImage;

out vec4 fragmentColor;

void main () {
    fragmentColor = texture(baseImage, vertexIn.texCoords);
}