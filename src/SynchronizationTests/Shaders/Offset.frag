/*

*/

// --- IN ---
layout(location = 0) in vec2 inTexCoords;

// --- OUT ---
layout(location = 0) out vec4 outColor;

// --- RESOURCES ---
MATERIAL(0, mapOffset)

// --- MAIN ---
void main() {
    outColor = vec4(texture(mapOffset, inTexCoords).rgb * 0.3, 0.0);
}
