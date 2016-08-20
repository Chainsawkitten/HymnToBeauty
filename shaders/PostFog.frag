/*
Fog based on Real-Time Fog using Post-processing in OpenGL (http://cs.gmu.edu/~jchen/cs662/fog.pdf).
*/
#version 400

#define M_E 2.718

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tExtra;
uniform sampler2D tDepth;

uniform vec3 color;
uniform mat4 inverseProjectionMatrix;
uniform float density;

out vec4 fragmentColor;

// Reconstruct position.
vec3 reconstructPos(vec2 texCoord, float depth){
    vec4 sPos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    sPos = inverseProjectionMatrix * sPos;

    return (sPos.xyz / sPos.w);
}

void main () {
    float depth = texture(tDepth, vertexIn.texCoords).r;
    gl_FragDepth = depth;
    
    vec3 position = reconstructPos(vertexIn.texCoords, depth);
    float z = length(position);
    float f = pow(M_E, -density * z);
    
    vec4 c = texture(tDiffuse, vertexIn.texCoords);
    fragmentColor = f * c + (1.0 - f) * vec4(color, 1.0);
}
