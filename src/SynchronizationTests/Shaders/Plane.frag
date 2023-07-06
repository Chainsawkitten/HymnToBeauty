/*
Forward shader shading lights and applying effects
*/

#include "Lighting.glsl"

// --- IN ---
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

// --- OUT ---
layout(location = 0) out vec4 outColor;

// --- RESOURCES ---
MATERIAL(0, mapAlbedo)
MATERIAL(1, mapOffset)

UNIFORMS
{
    uint lightCount;
    float gamma;
} uniforms;

STORAGE_BUFFER(1)
{
    Light lights[];
} lightBuffer;

vec3 ApplyPointLight(uint i, vec3 cameraToPos, vec3 albedo, vec3 normal, float metallic, float roughness, vec3 pos, vec3 F0) {
    // Point light
    const vec3 toLight = lightBuffer.lights[i].positionDistance.xyz - pos;
    vec3 surfaceToLight = normalize(toLight);
    float lightDist = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
    float attenuation = 1.0 / (1.0 + lightBuffer.lights[i].intensitiesAttenuation.w * lightDist);
    
    // Fade-out close to cutoff distance.
    lightDist = sqrt(lightDist) / lightBuffer.lights[i].positionDistance.w;
    const float curveTransition = 0.7;
    const float k = 1.0 / (1.0 - curveTransition);
    attenuation *= clamp(k - k * lightDist, 0.0, 1.0);

    // Calculate radiance of the light.
    vec3 radiance = lightBuffer.lights[i].intensitiesAttenuation.xyz * attenuation;
    
    // Add refraction.
    return ApplyLight(surfaceToLight, cameraToPos, radiance, albedo, normal, metallic, roughness, F0);
}

vec3 ApplyLights(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 pos) {
    vec3 Lo = vec3(0.0);
    vec3 cameraToPos = normalize(-pos);
    
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);
    
    // Point lights.
    for (uint i = 0; i < uniforms.lightCount; ++i) {
        Lo += ApplyPointLight(i, cameraToPos, albedo, normal, metallic, roughness, pos, F0);
    }

    return Lo;
}

// --- MAIN ---
void main() {
    vec3 albedo = texture(mapAlbedo, inTexCoords).rgb;
    albedo = pow(albedo, vec3(uniforms.gamma)); // Apply if texture not in sRGB
    vec3 pos = inPosition;

    // Shade fragment.
    vec3 color = ApplyLights(albedo, inNormal, 1.0, 1.0, pos);

    outColor = vec4(color, 1.0);
}
