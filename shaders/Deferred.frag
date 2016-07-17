/*
Lighting pass fragment shader (second pass)
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tNormals;
uniform sampler2D tSpecular;
uniform sampler2D tGlow;
uniform sampler2D tDepth;

uniform mat4 inverseProjectionMatrix;

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 extraOut;

// Reconstruct position.
vec3 ReconstructPos(vec2 texCoord, float depth){
    vec4 sPos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    sPos = inverseProjectionMatrix * sPos;

    return (sPos.xyz / sPos.w);
}

void main () {
    float depth = texture(tDepth, vertexIn.texCoords).r;
    vec3 position = ReconstructPos(vertexIn.texCoords, depth);
    vec3 diffuse = texture(tDiffuse, vertexIn.texCoords).rgb;
    vec3 normal = normalize(texture(tNormals, vertexIn.texCoords).xyz);
    vec3 specular = texture(tSpecular, vertexIn.texCoords).xyz;
    
    fragmentColor = vec4(diffuse, 1.0);
    extraOut = vec4(texture(tGlow, vertexIn.texCoords).rgb, 1.0);
    gl_FragDepth = depth;
}
