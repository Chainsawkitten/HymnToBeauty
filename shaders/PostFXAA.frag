/*
FXAA fragment shader.
*/
#version 400

in VertexData {
    vec2 texCoords;
} vertexIn;

uniform sampler2D tDiffuse;
uniform sampler2D tExtra;
uniform sampler2D tDepth;
uniform vec2 screenSize;

uniform float brightness;

const float FXAA_SPAN_MAX = 8.0;
const float FXAA_REDUCE_MUL = 1.0/8.0;
const float FXAA_REDUCE_MIN = 1.0/128.0;

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 extraOut;

vec3 InvGamma(vec3 color) {
    return pow(color, vec3(brightness));
}

vec3 Gamma(vec3 color) {
    return pow(color, vec3(1.0 / brightness));
}

void main () {
    vec2 invScreenSize = 1.0 / screenSize;
    
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaTL = dot(luma, Gamma(texture(tDiffuse, vertexIn.texCoords + (vec2(-1.0, -1.0) * invScreenSize)).rgb));
    float lumaTR = dot(luma, Gamma(texture(tDiffuse, vertexIn.texCoords + (vec2(1.0, -1.0) * invScreenSize)).rgb));
    float lumaBL = dot(luma, Gamma(texture(tDiffuse, vertexIn.texCoords + (vec2(-1.0, 1.0) * invScreenSize)).rgb));
    float lumaBR = dot(luma, Gamma(texture(tDiffuse, vertexIn.texCoords + (vec2(1.0, 1.0) * invScreenSize)).rgb));
    float lumaM  = dot(luma, Gamma(texture(tDiffuse, vertexIn.texCoords).rgb));

    vec2 dir;
    dir.x = -((lumaTL + lumaTR) - (lumaBL + lumaBR));
    dir.y = ((lumaTL + lumaBL) - (lumaTR + lumaBR));
    
    float dirReduce = max((lumaTL + lumaTR + lumaBL + lumaBR) * (FXAA_REDUCE_MUL * 0.25), FXAA_REDUCE_MIN);
    float inverseDirAdjustment = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), 
        max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * inverseDirAdjustment)) * invScreenSize;

    vec3 result1 = (1.0/2.0) * (
        Gamma(texture(tDiffuse, vertexIn.texCoords + (dir * vec2(1.0/3.0 - 0.5))).rgb) +
        Gamma(texture(tDiffuse, vertexIn.texCoords + (dir * vec2(2.0/3.0 - 0.5))).rgb));

    vec3 result2 = result1 * (1.0/2.0) + (1.0/4.0) * (
        Gamma(texture(tDiffuse, vertexIn.texCoords + (dir * vec2(0.0/3.0 - 0.5))).rgb) +
        Gamma(texture(tDiffuse, vertexIn.texCoords + (dir * vec2(3.0/3.0 - 0.5))).rgb));

    float lumaMin = min(lumaM, min(min(lumaTL, lumaTR), min(lumaBL, lumaBR)));
    float lumaMax = max(lumaM, max(max(lumaTL, lumaTR), max(lumaBL, lumaBR)));
    float lumaResult2 = dot(luma, result2);
    
    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
        fragmentColor = vec4(result1, 1.0);
    else
        fragmentColor = vec4(result2, 1.0);
    
    fragmentColor = vec4(InvGamma(fragmentColor.rgb), 1.0);

    float depth = texture(tDepth, vertexIn.texCoords).r;
    gl_FragDepth = depth;
    
    extraOut = texture(tExtra, vertexIn.texCoords);
}