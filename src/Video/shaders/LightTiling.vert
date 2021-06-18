/*
Tile lights using HW rasterization.
*/

layout(location = 0) in vec3 position;

layout(location = 0) flat out uint instance;

MATRICES
{
    mat4 projectionMatrix;
    vec2 scale;
    float invIsocahedronError;
} matrices;

struct Light {
    vec4 positionDistance;
    vec4 intensitiesAttenuation;
    vec4 directionConeAngle;
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
    
    // Adjust to account for tiles not lining up perfectly with screen size.
    gl_Position /= gl_Position.w;
    gl_Position *= vec4(matrices.scale, 1.0, 1.0);
    vec2 offset = vec2(-(1.0 - matrices.scale.x), 1.0 - matrices.scale.y);
#ifdef VULKAN
    offset.y = -offset.y;
#endif
    gl_Position += vec4(offset, 0.0, 0.0);
}
