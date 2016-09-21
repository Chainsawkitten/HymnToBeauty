/*
Passthrough vertex shader
*/
#version 400
layout(location = 0) in vec3 particlePosition;
layout(location = 1) in vec2 particleSize;
layout(location = 2) in float particleLife;
layout(location = 3) in float particleLifetime;
layout(location = 4) in vec3 particleVelocity;
layout(location = 5) in vec3 alpha;
layout(location = 6) in vec3 color;
layout(location = 7) in float textureIndex;

out VertexData {
    float alpha;
    vec2 size;
    vec3 color;
    float textureIndex;
} vertexOut;

void main () {
    float a = 2.0 * particleLife / particleLifetime;
    float alphArr[3] = float[](alpha.x, alpha.y, alpha.z);
    vertexOut.alpha = (1.0 - fract(a)) * alphArr[int(a)] + fract(a) * alphArr[int(a)+1];
    vertexOut.size = particleSize;
    vertexOut.color = color;
    vertexOut.textureIndex = textureIndex;
    gl_Position = vec4(particlePosition + particleVelocity * particleLife, 1.0);
}