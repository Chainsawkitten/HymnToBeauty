/*
Tile lights using HW rasterization.
*/

layout(location = 0) flat in uint instance;

UNIFORMS
{
    uint tileSize;
    uint maskCount;
    uint tilesX;
} uniforms;

STORAGE_BUFFER_RW(1)
{
    highp uint masks[];
} tileMaskBuffer;

void main() {
    // Calculate which tile we're in.
    const uvec2 tile = uvec2(gl_FragCoord.xy) / (uniforms.tileSize / 2u);
    const uint tileIndex = tile.x + tile.y * uniforms.tilesX;
    
    // Set the light's bit in the tile's light mask.
	const uint lightMask = 0;//instance / 32u;
	const highp uint lightIndex = instance % 32u;
	
	const highp uint mask = 1u << lightIndex;
	atomicOr(tileMaskBuffer.masks[tileIndex * uniforms.maskCount + lightMask], mask);
}
