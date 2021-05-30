/*
Downscales and applies luminance threshold.
*/
layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

precision mediump float;

MATERIAL(0, colorSampler)

PUSH_CONSTANTS
{
    float threshold;
} pushConst;

void main () {
    vec3 color = texture(colorSampler, inTexCoords).rgb;
    
    // https://en.wikipedia.org/wiki/Relative_luminance
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    outColor = luminance > pushConst.threshold ? vec4(color, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);
}
