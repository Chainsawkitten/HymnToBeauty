/*
Gamma correction.
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tDepth;

uniform float brightness;

out vec4 fragmentColor;

void main () {
    vec3 color = texture(tDiffuse, vertexIn.texCoords).rgb;
    vec3 gamma = vec3(brightness);
    fragmentColor = vec4(pow(color, gamma), 1.0);
    float depth = texture(tDepth, vertexIn.texCoords).r;
    gl_FragDepth = depth;
}
