/*
Render a texture to the screen.
*/
#version 400

in VertexData {
	vec2 texCoords;
} vertexIn;

uniform sampler2D baseImage;

void main () {
    gl_FragColor = texture(baseImage, vertexIn.texCoords);
}