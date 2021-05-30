/*
Applies various post-processing effects.
*/
layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)
MATERIAL(1, bloomSampler)

UNIFORMS
{
    float gamma;
    float bloomIntensity;
} uniforms;

void main () {
    vec3 color = texture(colorSampler, inTexCoords).rgb;
    
    // Bloom.
    if (uniforms.bloomIntensity > 0.0) {
        color += uniforms.bloomIntensity * texture(bloomSampler, inTexCoords).rgb;
    }
    
    // Reinhard tone mapping.
    color = clamp(color / (color + vec3(1.0)), 0.0, 1.0);
    
    // Gamma correction.
    color = pow(color, vec3(1.0 / uniforms.gamma));
    
    outColor = vec4(color, 1.0);
}
