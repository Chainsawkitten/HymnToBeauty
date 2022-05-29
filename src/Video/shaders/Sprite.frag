layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in flat mediump vec4 inTint;

layout(location = 0) out mediump vec4 outColor;

MATERIAL(0, colorSampler)

UNIFORMS
{
    mediump float gamma;
} uniforms;

void main() {
    mediump vec4 color = texture(colorSampler, inTexCoords);
    color.rgb = pow(color.rgb, vec3(uniforms.gamma)); // Apply if texture not in sRGB
    outColor = color * inTint;
    
    // Pre-multiply alpha.
    outColor = vec4(outColor.rgb * outColor.a, outColor.a);
}
