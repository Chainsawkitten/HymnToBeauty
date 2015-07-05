#version 400

in VertexData {
	vec2 texCoords;
} vertexIn;

uniform vec3 color;

void main () {
    gl_FragColor = vec4(color, 1.0);
}