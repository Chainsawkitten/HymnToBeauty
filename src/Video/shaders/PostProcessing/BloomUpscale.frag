/*
Upscales bloom texture.
*/
layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)
MATERIAL(1, downscaledSampler)

PUSH_CONSTANTS
{
    float scatter;
} pushConst;

void main () {
    vec3 color = mix(texture(colorSampler, inTexCoords).rgb, texture(downscaledSampler, inTexCoords).rgb, pushConst.scatter);
    
    outColor = vec4(color, 1.0);
}
