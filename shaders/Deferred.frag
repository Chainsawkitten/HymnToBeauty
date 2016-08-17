/*
Lighting pass fragment shader (second pass)
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tNormals;
uniform sampler2D tSpecular;
uniform sampler2D tGlow;
uniform sampler2D tDepth;

uniform mat4 inverseProjectionMatrix;

struct Light {
    vec4 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
    float coneAngle;
    vec3 direction;
};

const int maxLights = 32;
uniform int lightCount;
uniform Light lights[maxLights];

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 extraOut;

// Apply ambient, diffuse and specular lighting.
vec3 ApplyLight(in vec3 surfaceColor, in vec3 normal, in vec3 position, in vec3 surfaceSpecular, in int light) {
    vec3 surfaceToLight;
    float attenuation = 1.0;
    
    if (lights[light].position.w == 0.0) {
        // Directional light.
        surfaceToLight = normalize(lights[light].position.xyz);
        attenuation = 1.0;
    } else {
        // Point light.
        vec3 toLight = lights[light].position.xyz - position;
        surfaceToLight = normalize(toLight);
        attenuation = 1.0 / (1.0 + lights[light].attenuation * (toLight.x * toLight.x + toLight.y * toLight.y + toLight.z * toLight.z));
        
        // Spot light.
        float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(lights[light].direction))));
        if (lightToSurfaceAngle > lights[light].coneAngle)
            attenuation = 0.0;
    }
    
    // Ambient.
    vec3 ambient = lights[light].ambientCoefficient * surfaceColor;
    
    // Diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor * lights[light].intensities;
    
    // Specular
    vec3 surfaceToCamera = normalize(-position);
    float materialShininess = 20.0;
    float specularCoefficient = 0.0;
    if (diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(dot(surfaceToCamera, reflect(-surfaceToLight, normal)), 0.0), materialShininess);
    vec3 specular = specularCoefficient * surfaceSpecular * lights[light].intensities;
    
    // Linear color (color before gamma correction)
    return ambient + attenuation * (diffuse + specular);
}

// Reconstruct position.
vec3 ReconstructPos(in vec2 texCoord, in float depth){
    vec4 sPos = vec4(texCoord * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);
    sPos = inverseProjectionMatrix * sPos;

    return (sPos.xyz / sPos.w);
}

void main () {
    float depth = texture(tDepth, vertexIn.texCoords).r;
    vec3 position = ReconstructPos(vertexIn.texCoords, depth);
    vec3 diffuse = texture(tDiffuse, vertexIn.texCoords).rgb;
    vec3 normal = normalize(texture(tNormals, vertexIn.texCoords).xyz);
    vec3 specular = texture(tSpecular, vertexIn.texCoords).xyz;
    
    vec3 accumulatedLight;
    for (int i=0; i < lightCount; ++i) {
        accumulatedLight += ApplyLight(diffuse, normal, position, specular, i);
    }
    
    fragmentColor = vec4(accumulatedLight, 1.0);
    extraOut = vec4(texture(tGlow, vertexIn.texCoords).rgb, 1.0);
    gl_FragDepth = depth;
}
