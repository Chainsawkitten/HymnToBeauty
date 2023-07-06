/*
Simple pass-through vertex shader - Vertex Shader
*/
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

layout(location = 0) out vec2 outTexCoords;

struct PerInstance {
    mat4 modelMatrix;
};

STORAGE_BUFFER(0)
{
    PerInstance data[];
} instances;

void main () {
    gl_Position = instances.data[InstanceIndex].modelMatrix * vec4(vertexPosition, 1.0);
    outTexCoords = vertexTexture;
    
    FixPosition();
}
