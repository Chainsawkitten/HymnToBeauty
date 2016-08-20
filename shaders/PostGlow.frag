/*
Second pass glow filter. Applies the glow to the screen.
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tExtra;
uniform sampler2D tDepth;

out vec4 fragmentColor;

void main () {
    fragmentColor = texture(tDiffuse, vertexIn.texCoords) + texture(tExtra, vertexIn.texCoords);

    float depth = texture(tDepth, vertexIn.texCoords).r;
    gl_FragDepth = depth;
}