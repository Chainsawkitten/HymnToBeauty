const float PI = 3.14159265359f;

// Pseudo-random number.
highp float rand(vec2 co) {
    highp float a = 12.9898f;
    highp float b = 78.233f;
    highp float c = 43758.5453f;
    highp float dt = dot(co.xy, vec2(a,b));
    highp float sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

// Applies a low-amplitude noise to avoid color banding.
vec3 dither(vec3 color, vec2 texCoords, highp float time) {
    float noise = rand(texCoords + vec2(time, 0.0f)) / 255.0f;
    return color + vec3(noise);
}
