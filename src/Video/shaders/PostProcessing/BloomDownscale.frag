/*
Downscales.
*/
layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 outColor;

MATERIAL(0, colorSampler)

void main () {
    vec3 color = texture(colorSampler, inTexCoords).rgb;
    
    outColor = vec4(color, 1.0);
}
