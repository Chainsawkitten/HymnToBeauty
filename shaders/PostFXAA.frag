/*
FXAA fragment shader.
*/
#version 400

// --- IN ---
in VertexData {
    vec2 texCoords;
} vertexIn;

// --- OUT ---
layout(location = 0) out vec4 fragmentColor;

// --- UNIFORMS ---
uniform sampler2D tDiffuse;
uniform vec2 screenSize;

// --- CONSTANTS ---
const float FXAA_SPAN_MAX = 8.0f;
const float FXAA_REDUCE_MUL = 1.0f/8.0f;
const float FXAA_REDUCE_MIN = 1.0f/128.0f;

// --- MAIN ---
void main () {
    vec2 invScreenSize = 1.0f / screenSize;
    
    vec3 luma = vec3(0.299f, 0.587f, 0.114f);
    float lumaTL = dot(luma, texture(tDiffuse, vertexIn.texCoords + (vec2(-1.0f, -1.0f) * invScreenSize)).rgb);
    float lumaTR = dot(luma, texture(tDiffuse, vertexIn.texCoords + (vec2(1.0f, -1.0f) * invScreenSize)).rgb);
    float lumaBL = dot(luma, texture(tDiffuse, vertexIn.texCoords + (vec2(-1.0f, 1.0f) * invScreenSize)).rgb);
    float lumaBR = dot(luma, texture(tDiffuse, vertexIn.texCoords + (vec2(1.0f, 1.0f) * invScreenSize)).rgb);
    float lumaM  = dot(luma, texture(tDiffuse, vertexIn.texCoords).rgb);

    vec2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
    
    float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (FXAA_REDUCE_MUL * 0.25f), FXAA_REDUCE_MIN);
    float inverseDirAdjustment = 1.0f / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), 
        max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * inverseDirAdjustment)) * invScreenSize;

    vec3 result1 = (1.0f/2.0f) * (
        texture(tDiffuse, vertexIn.texCoords + (dir * vec2(1.0f/3.0f - 0.5f))).rgb +
        texture(tDiffuse, vertexIn.texCoords + (dir * vec2(2.0f/3.0f - 0.5f))).rgb);

    vec3 result2 = result1 * (1.0f/2.0f) + (1.0f/4.0f) * (
        texture(tDiffuse, vertexIn.texCoords + (dir * vec2(0.0f/3.0f - 0.5f))).rgb +
        texture(tDiffuse, vertexIn.texCoords + (dir * vec2(3.0f/3.0f - 0.5f))).rgb);

    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaResult2 = dot(luma, result2);
    
    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
        fragmentColor = vec4(result1, 1.0f);
    else
        fragmentColor = vec4(result2, 1.0f);
}
