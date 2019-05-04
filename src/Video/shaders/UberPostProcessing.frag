/*
Applies various post-processing effects.
*/
layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)

FRAGMENT_UNIFORMS
{
    float gamma;
} uniforms;

void main () {
    vec3 color = texture(colorSampler, inTexCoords).rgb;
    
    // Reinhard tone mapping.
    color = clamp(color / (color + vec3(1.0f)), 0.0f, 1.0f);
    
    // Gamma correction.
    color = pow(color, vec3(1.0f / uniforms.gamma));
    
    outColor = vec4(color, 1.0);
}
