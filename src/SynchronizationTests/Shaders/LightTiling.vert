/*
Tile lights using HW rasterization.
*/

layout(location = 0) in vec3 position;

layout(location = 0) flat out uint instance;

MATRICES
{
    mat4 projectionMatrix;
    float invIsocahedronError;
} matrices;

struct Light {
    vec4 positionDistance;
    vec4 intensitiesAttenuation;
};

STORAGE_BUFFER(0)
{
    Light lights[];
} lightBuffer;

void main() {
    const vec3 sphereOrigin = lightBuffer.lights[InstanceIndex].positionDistance.xyz;
	const float radius = lightBuffer.lights[InstanceIndex].positionDistance.w * matrices.invIsocahedronError;
    gl_Position = matrices.projectionMatrix * vec4(position * radius + sphereOrigin, 1.0);
    instance = InstanceIndex;

    FixPosition();
    
    gl_Position /= gl_Position.w;
}
