/*
Linearly separable 9x9 gaussian blur.
*/
layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

precision mediump float;

MATERIAL(0, colorSampler)

// 9x9, sigma=1.8, done as 5 taps with bilinear filtering.
const int sampleCount = 3;
const float weights[sampleCount] = float[](0.224174, 0.313037, 0.074876);
const float offsets[sampleCount] = float[](0.0, 1.386283, 3.253459);

void main () {
    vec3 color = weights[0] * texture(colorSampler, inTexCoords).rgb;
    
    const vec2 offset = 1.0 / textureSize(colorSampler, 0);
    
    color += weights[1] * texture(colorSampler, inTexCoords + offsets[1] * offset).rgb;
    color += weights[1] * texture(colorSampler, inTexCoords - offsets[1] * offset).rgb;
    
    color += weights[2] * texture(colorSampler, inTexCoords + offsets[2] * offset).rgb;
    color += weights[2] * texture(colorSampler, inTexCoords - offsets[2] * offset).rgb;
    
    outColor = vec4(color, 1.0);
}
