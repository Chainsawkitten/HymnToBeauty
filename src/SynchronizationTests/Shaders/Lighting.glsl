// --- STRUCTS ---
struct Light {
    vec4 positionDistance;
    vec4 intensitiesAttenuation;
};

// --- CONSTANTS ---
const float PI = 3.14159265359f;

// --- SHADING FUNCTIONS ---

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
