#version 400

in VertexData {
	vec2 texCoords;
} vertexIn;

uniform sampler2D baseImage;
uniform vec3 color;

out vec4 fragmentColor;

void main () {
    fragmentColor = vec4(color, texture(baseImage, vertexIn.texCoords).a);
}