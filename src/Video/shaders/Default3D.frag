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
struct Light {
    vec4 position;
    vec3 intensities;
    float attenuation;
    vec3 direction;
    float ambientCoefficient;
    float coneAngle;
    float distance;
    float padding;
};

// --- RESOURCES ---
MATERIAL(0, mapAlbedo)
MATERIAL(1, mapNormal)
MATERIAL(2, mapRoughnessMetallic)

UNIFORMS
{
    int lightCount;
    float gamma;
} uniforms;

STORAGE_BUFFER
{
    Light lights[];
} lightBuffer;

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
    return TBN * mn;
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

vec3 ApplyLights(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 pos) {
    vec3 Lo = vec3(0.0f);
    vec3 N = normalize(normal);
    vec3 V = normalize(-pos);
    
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);
    
    for(int i = 0; i < uniforms.lightCount; i++) {
        vec3 surfaceToLight;
        float attenuation;
        float shadow = 0.0;

        if (lightBuffer.lights[i].position.w == 0.0f) {
            //Directional light.
            surfaceToLight = normalize(lightBuffer.lights[i].position.xyz);
            attenuation = 1.0f;
        } else {
            // Point light
            vec3 toLight = lightBuffer.lights[i].position.xyz - pos;
            surfaceToLight = normalize(toLight);
            float lightDist = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
            attenuation = 1.0 / (1.0 + lightBuffer.lights[i].attenuation * lightDist);
            
            // Fade-out close to cutoff distance.
            lightDist = sqrt(lightDist) / lightBuffer.lights[i].distance;
            const float curveTransition = 0.7;
            const float k = 1.0 / (1.0 - curveTransition);
            attenuation *= clamp(k - k * lightDist, 0.0, 1.0);
            
            // Spot light.
            if (lightBuffer.lights[i].coneAngle < 179.0) {
                float lightToSurfaceAngle = degrees(acos(clamp(dot(-surfaceToLight, normalize(lightBuffer.lights[i].direction)), -1.0, 1.0)));
                float fadeLength = 10.0;
                if (lightToSurfaceAngle > lightBuffer.lights[i].coneAngle - fadeLength) {
                    attenuation *= 1.0 - clamp(lightToSurfaceAngle - (lightBuffer.lights[i].coneAngle - fadeLength), 0.0, fadeLength) / fadeLength;
                }
            }
        }

        vec3 H = normalize(V + surfaceToLight);

        // Calculate radiance of the light.
        vec3 radiance = lightBuffer.lights[i].intensities * attenuation;
        
        // Cook-torrance brdf.
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, surfaceToLight, roughness);
        vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
        
        // Calculate specular.
        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, surfaceToLight), 0.0) + 0.001;
        vec3 specular = (nominator / denominator);
        
        // Energy of light that gets reflected.
        vec3 kS = F;
        
        // Energy of light that gets refracted (no refraction occurs when metallic).
        vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
        
        // Calculate light contribution.
        float NdotL = max(dot(N, surfaceToLight), 0.0f);
        
        // Add refraction.
        Lo += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
        
        // Add ambient.
        Lo += lightBuffer.lights[i].ambientCoefficient * albedo;
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
