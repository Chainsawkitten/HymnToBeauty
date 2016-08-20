/*
First pass glow filter. Blurs along the specified axis.
*/
#version 400

uniform sampler2D tDiffuse;
uniform sampler2D tExtra;
uniform sampler2D tDepth;

uniform float horizontal;
uniform vec2 screenSize;

uniform float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

in vec2 texCoords;

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 extraOut;

void main () {
    fragmentColor = texture(tDiffuse, texCoords);
    extraOut = texture(tExtra, texCoords) * weight[0];
    
    vec2 texOffset = vec2(horizontal, 1.0 - horizontal) / screenSize;
    for (int i=1; i<5; ++i) {
        extraOut += texture(tExtra, texCoords + texOffset * i) * weight[i];
        extraOut += texture(tExtra, texCoords - texOffset * i) * weight[i];
    }

    float depth = texture(tDepth, texCoords).r;
    gl_FragDepth = depth;
}