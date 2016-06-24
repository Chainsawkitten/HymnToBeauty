/*
Copy fragment shader.
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tDepth;

out vec4 fragmentColor;

void main () {
    fragmentColor = texture(tDiffuse, vertexIn.texCoords);
    gl_FragDepth = texture(tDepth, vertexIn.texCoords).r;
}
