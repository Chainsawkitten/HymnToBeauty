#version 400

in VertexData {
	vec2 texCoords;
} vertexIn;

uniform sampler2D baseImage;
uniform vec3 color;

void main () {
    gl_FragColor = vec4(color, texture(baseImage, vertexIn.texCoords).a);
}