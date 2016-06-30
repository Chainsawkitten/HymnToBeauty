/*
Simple pass-through vertex shader - Vertex Shader
*/
#version 400
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

uniform mat4 viewProjection;
uniform mat4 model;
uniform mat3 normalMatrix;

out VertexData {
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
} vertexOut;

void main () {
    gl_Position = viewProjection * (model * vec4(vertexPosition, 1.0));
    vertexOut.normal = normalize(normalMatrix * vertexNormal);
    vertexOut.tangent = vertexTangent;
    vertexOut.texCoords = vertexTexture;
}
