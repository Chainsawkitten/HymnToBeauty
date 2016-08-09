/*
Geometry pass fragment shader (first pass).
*/
#version 400

in VertexData {
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
} vertexIn;

uniform sampler2D baseImage;
uniform sampler2D normalMap;
uniform sampler2D specularMap;
uniform sampler2D glowMap;

layout(location = 0) out vec3 diffuseOut;
layout(location = 1) out vec3 normalsOut;
layout(location = 2) out vec3 specularOut;
layout(location = 3) out vec3 glowOut;

// Calculate normal based on interpolated vertex normal, sampled normal (from normal map) and vertex tangent.
vec3 calculateNormal(in vec3 normal, in vec3 tangent, in vec3 mapNormal) {
    vec3 n = normalize(normal);
    vec3 t = normalize(tangent);
    t = normalize(t - dot(t, n) * n);
    vec3 b = cross(t, n);
    if (dot(cross(n, t), b) < 0.0)
        t = -t;
    
    vec3 mn = normalize(2.0 * mapNormal - vec3(1.0, 1.0, 1.0));
    mat3 TBN = mat3(t, b, n);
    return TBN * mn;
}

void main() {
    diffuseOut = texture(baseImage, vertexIn.texCoords).rgb;
    normalsOut = calculateNormal(vertexIn.normal, vertexIn.tangent, texture(normalMap, vertexIn.texCoords).rgb);
    specularOut = texture(specularMap, vertexIn.texCoords).rgb;
    glowOut = texture(glowMap, vertexIn.texCoords).rgb * diffuseOut;
}
