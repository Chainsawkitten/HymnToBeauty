/*
Adjust color fragment shader.
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tExtra;
uniform sampler2D tDepth;

uniform vec3 color;

out vec4 fragmentColor;

void main () {
    fragmentColor = texture(tDiffuse, vertexIn.texCoords) * vec4(color, 1.0);

    float depth = texture(tDepth, vertexIn.texCoords).r;
    gl_FragDepth = depth;
}
