/*
Forward shader shading lights and applying effects
*/

// --- IN ---
layout(location = 0) in VertexData {
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
} vertexIn;

// --- OUT ---
layout(location = 0) out vec4 outColor;

// --- STRUCTS ---
struct DirectionalLight {
    vec4 direction;
    vec4 intensitiesAmbientCoefficient;
};

struct Light {
    vec4 positionDistance;
    vec4 intensitiesAttenuation;
    vec4 directionConeAngle;
};

// --- RESOURCES ---
MATERIAL(0, mapAlbedo)
MATERIAL(1, mapNormal)
MATERIAL(2, mapRoughnessMetallic)

UNIFORMS
{
    uint directionalLightCount;
    uint lightCount;
    uint maskCount;
    uint zBins;
    uint tileSize;
    uint tilesX;
    float gamma;
    float zNear;
    float zFar;
} uniforms;

STORAGE_BUFFER(0)
{
    DirectionalLight lights[];
} directionalLightBuffer;

STORAGE_BUFFER(1)
{
    Light lights[];
} lightBuffer;

STORAGE_BUFFER(2)
{
    highp uint masks[];
} zMaskBuffer;

STORAGE_BUFFER(3)
{
    highp uint masks[];
} tileMaskBuffer;

// --- CONSTANTS ---
const float PI = 3.14159265359f;

// --- SHADING FUNCTIONS ---
// Calculate normal based on interpolated vertex normal, sampled normal (from normal map) and vertex tangent.
vec3 calculateNormal(in vec3 normal, in vec3 tangent, in vec3 normalFromMap) {
    vec3 n = normalize(normal);
    vec3 t = normalize(tangent);
    t = normalize(t - dot(t, n) * n);
    vec3 b = cross(t, n);
    if (dot(cross(n, t), b) < 0.0)
        t = -t;
    
    vec3 mn = 2.0 * normalFromMap - vec3(1.0, 1.0, 1.0);
    mat3 TBN = mat3(t, b, n);
    return normalize(TBN * mn);
}

//Calc ratio between specular and diffuse reflection, surface reflections
vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    cosTheta = 1.0f - cosTheta;
    return F0 + (1.0f - F0) * cosTheta * cosTheta * cosTheta * cosTheta * cosTheta;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom =(NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;
    
    return clamp((nom / denom), 0.0f, 5.5f);
}

float GemetrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N,V), 0.0f);
    float NdotL = max(dot(N,L), 0.0f);
    float ggx1 = GemetrySchlickGGX(NdotV, roughness);
    float ggx2 = GemetrySchlickGGX(NdotL, roughness);

    return clamp((ggx1 * ggx2), 0.f, 1.0f);
}

vec3 ApplyLight(vec3 surfaceToLight, vec3 cameraToPos, vec3 radiance, vec3 albedo, vec3 normal, float metallic, float roughness, vec3 F0) {
    vec3 H = normalize(cameraToPos + surfaceToLight);
    
    // Cook-torrance brdf.
    float NDF = DistributionGGX(normal, H, roughness);
    float G = GeometrySmith(normal, cameraToPos, surfaceToLight, roughness);
    vec3 F = FresnelSchlick(max(dot(H, cameraToPos), 0.0), F0);
    
    // Calculate specular.
    vec3 nominator = NDF * G * F;
    float denominator = 4.0 * max(dot(normal, cameraToPos), 0.0) * max(dot(normal, surfaceToLight), 0.0) + 0.001;
    vec3 specular = (nominator / denominator);
    
    // Energy of light that gets reflected.
    vec3 kS = F;
    
    // Energy of light that gets refracted (no refraction occurs when metallic).
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    
    // Calculate light contribution.
    float NdotL = max(dot(normal, surfaceToLight), 0.0f);
    
    // Add refraction.
    return (kD * albedo / PI + specular) * radiance * NdotL; // TODO Add shadow here.
}

vec3 ApplyDirectionalLight(uint i, vec3 cameraToPos, vec3 albedo, vec3 normal, float metallic, float roughness, vec3 F0) {
    vec3 surfaceToLight = normalize(directionalLightBuffer.lights[i].direction.xyz);

    // Calculate radiance of the light.
    vec3 radiance = directionalLightBuffer.lights[i].intensitiesAmbientCoefficient.xyz;
    
    // Add refraction.
    vec3 Lo = ApplyLight(surfaceToLight, cameraToPos, radiance, albedo, normal, metallic, roughness, F0);
    
    // Add ambient.
    Lo += directionalLightBuffer.lights[i].intensitiesAmbientCoefficient.w * albedo;
    
    return Lo;
}

vec3 ApplySpotPointLight(uint i, vec3 cameraToPos, vec3 albedo, vec3 normal, float metallic, float roughness, vec3 pos, vec3 F0) {
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
    
    // Spot light.
    if (lightBuffer.lights[i].directionConeAngle.w < 179.0) {
        float lightToSurfaceAngle = degrees(acos(clamp(dot(-surfaceToLight, normalize(lightBuffer.lights[i].directionConeAngle.xyz)), -1.0, 1.0)));
        float fadeLength = 10.0;
        if (lightToSurfaceAngle > lightBuffer.lights[i].directionConeAngle.w - fadeLength) {
            attenuation *= 1.0 - clamp(lightToSurfaceAngle - (lightBuffer.lights[i].directionConeAngle.w - fadeLength), 0.0, fadeLength) / fadeLength;
        }
    }

    // Calculate radiance of the light.
    vec3 radiance = lightBuffer.lights[i].intensitiesAttenuation.xyz * attenuation;
    
    // Add refraction.
    return ApplyLight(surfaceToLight, cameraToPos, radiance, albedo, normal, metallic, roughness, F0);
}

vec3 ApplyLights(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 pos) {
    vec3 Lo = vec3(0.0);
    vec3 cameraToPos = normalize(-pos);
    
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);
    
    // Directional lights.
    for(int i = 0; i < uniforms.directionalLightCount; i++) {
        Lo += ApplyDirectionalLight(i, cameraToPos, albedo, normal, metallic, roughness, F0);
    }
    
    // Spot and point lights.
    const uvec2 tile = uvec2(gl_FragCoord.xy) / uniforms.tileSize;
    const uint tileIndex = tile.x + tile.y * uniforms.tilesX;
    const uint z = min(uint(sqrt(max((-pos.z - uniforms.zNear) / (uniforms.zFar - uniforms.zNear), 0.0)) * float(uniforms.zBins)), uniforms.zBins - 1);
    for (uint i = 0; i < uniforms.maskCount; ++i) {
        const highp uint maskTile = tileMaskBuffer.masks[tileIndex * uniforms.maskCount + i];
        const highp uint maskZ = zMaskBuffer.masks[z * uniforms.maskCount + i];
		highp uint mask = maskTile & maskZ;
        
        while (mask != 0u) {
			highp uint bitIndex = findLSB(mask);
			Lo += ApplySpotPointLight(i * 32u + bitIndex, cameraToPos, albedo, normal, metallic, roughness, pos, F0);
			mask ^= (1u << bitIndex);
		}
    }

    return Lo;
}

// --- MAIN ---
void main() {
    vec3 albedo = texture(mapAlbedo, vertexIn.texCoords).rgb;
    albedo = pow(albedo, vec3(uniforms.gamma)); // Apply if texture not in sRGB
    vec3 normal = calculateNormal(vertexIn.normal, vertexIn.tangent, texture(mapNormal, vertexIn.texCoords).rgb);
    vec2 roughnessMetallic = texture(mapRoughnessMetallic, vertexIn.texCoords).gb;
    vec3 pos = vertexIn.pos;

    // Shade fragment.
    vec3 color = ApplyLights(albedo, normal, roughnessMetallic.r, roughnessMetallic.g, pos);

    outColor = vec4(color, 1.0f);
}
