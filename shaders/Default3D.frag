/*
Forward shader shading lights and applying effects
*/
#version 430

// --- IN ---
in VertexData {
    vec3 pos;
    vec3 normal;
    vec3 tangent;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} vertexIn;

// --- OUT ---
layout(location = 0) out vec4 fragmentColor;

// --- STRUCTS ---
struct Light {
    vec4 position;
    vec3 intensities;
    float attenuation;
    vec3 direction;
    float ambientCoefficient;
    float coneAngle;
    float shadow;
    float distance;
    float padding;
};

// --- BINDINGS ---
layout(std430, binding = 5) buffer bBuffer { Light lights[]; };

// UNIFORMS
// Shading uniforms.
uniform int lightCount;
uniform sampler2D mapAlbedo;
uniform sampler2D mapNormal;
uniform sampler2D mapMetallic;
uniform sampler2D mapRoughness;
uniform sampler2D tDepth;
uniform sampler2D mapShadow;
// Image processing uniforms.
uniform float cameraNear;
uniform float cameraFar;
uniform float gamma;
uniform bool fogApply;
uniform float fogDensity;
uniform vec3 fogColor;
uniform bool colorFilterApply;
uniform vec3 colorFilterColor;
uniform bool ditherApply;
uniform float time;
uniform vec2 frameSize;

// --- CONSTANTS ---
const float PI = 3.14159265359f;
const float M_E = 2.718f;

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
// --- SHADOW FUNCTIONS ----
float ShadowCalculation(vec4 fragPosLightSpace){
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // to range [0,1]
    projCoords = projCoords * 0.5 + vec3(0.5);
    float closestDepth = texture(mapShadow, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth  > closestDepth ? 1.0 : 0.0;

    return shadow;
}

vec3 ApplyLights(vec3 albedo, vec3 normal, float metallic, float roughness, vec3 pos) {
    vec3 Lo = vec3(0.0f);
    vec3 N = normalize(normal);
    vec3 V = normalize(-pos);
    
    vec3 F0 = mix(vec3(0.04f), albedo, metallic);
    
    for(int i = 0; i < lightCount; i++) {
        vec3 surfaceToLight;
        float attenuation;
        float shadow = 0.0;

        if (lights[i].position.w == 0.0f) {
            //Directional light.
            surfaceToLight = normalize(lights[i].position.xyz);
            attenuation = 1.0f;
        } else {
            // Point light
            vec3 toLight = lights[i].position.xyz - pos;
            surfaceToLight = normalize(toLight);
            float lightDist = toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z;
            attenuation = 1.0 / (1.0 + lights[i].attenuation * lightDist);
            
            // Fade-out close to cutoff distance.
            lightDist = sqrt(lightDist) / lights[i].distance;
            const float curveTransition = 0.7;
            const float k = 1.0 / (1.0 - curveTransition);
            attenuation *= clamp(k - k * lightDist, 0.0, 1.0);
            
            // Spot light.
            if (lights[i].coneAngle < 179.0) {
                if(lights[i].shadow > 0.1)
                    shadow = ShadowCalculation(vertexIn.fragPosLightSpace);
                
                float lightToSurfaceAngle = degrees(acos(clamp(dot(-surfaceToLight, normalize(lights[i].direction)), -1.0, 1.0)));
                float fadeLength = 10.0;
                if (lightToSurfaceAngle > lights[i].coneAngle - fadeLength) {
                    attenuation *= 1.0 - clamp(lightToSurfaceAngle - (lights[i].coneAngle - fadeLength), 0.0, fadeLength) / fadeLength;
                }
            }
        }

        vec3 H = normalize(V + surfaceToLight);

        // Calculate radiance of the light.
        vec3 radiance = lights[i].intensities * attenuation;
        
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
        Lo += lights[i].ambientCoefficient * albedo;
    }

    return Lo;
}

// --- POSTPROCESSING FUNCTIONS ---
vec3 ApplyFog(vec3 pos, vec3 color) {
    float z = length(pos);
    float f = pow(M_E, -fogDensity * z);
    return f * color + (1.0 - f) * fogColor;
}

highp float rand(vec2 co) {
    highp float a = 12.9898f;
    highp float b = 78.233f;
    highp float c = 43758.5453f;
    highp float dt = dot(co.xy, vec2(a,b));
    highp float sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

// --- MAIN ---
void main() {

    vec3 finalColor = vec3(0.f, 0.f, 0.f);
    
    float depth = texture(tDepth, vertexIn.texCoords).r;
    vec3 albedo = texture(mapAlbedo, vertexIn.texCoords).rgb;
    albedo = pow(albedo, vec3(gamma)); // Apply if texture not in sRGB
    vec3 normal = calculateNormal(vertexIn.normal, vertexIn.tangent, texture(mapNormal, vertexIn.texCoords).rgb);
    float metallic = texture(mapMetallic, vertexIn.texCoords).r;
    float roughness = texture(mapRoughness, vertexIn.texCoords).r;
    vec3 pos = vertexIn.pos;

    // Shade fragment.
    vec3 color = ApplyLights(albedo, normal, metallic, roughness, pos);

    // Fog.
    if (fogApply)
        color = ApplyFog(pos, color);
        
    // Color Filter.
    if (colorFilterApply)
        color = color * colorFilterColor;
    
    // Reinhard tone mapping.
    color = clamp(color / (color + vec3(1.0f)), 0.0f, 1.0f);
    
    // Gamma correction.
    color = pow(color, vec3(1.0f / gamma));

	// Dither.
	if (ditherApply) {
		float dither = rand(gl_FragCoord.xy / frameSize + vec2(time, 0.0f)) / 255.0f;
		color = color + vec3(dither);
	}

    float linDepth = (2 * cameraNear) / (cameraFar + cameraNear - gl_FragCoord.z * (cameraFar - cameraNear));
    float depthTwo = linDepth * linDepth;
    float depthFour = depthTwo * depthTwo;
    float depthSixteen = depthFour * depthFour;
    color *= (1.0f - depthSixteen);

    // Final color.
    finalColor = color;

    fragmentColor = vec4(finalColor, 1.0f);
}
