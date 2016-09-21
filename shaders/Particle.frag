#version 400
in VertexData {
    vec2 texCoords;
    float alpha;
    vec3 color;
} vertexIn;

uniform sampler2D baseImage;

layout(location = 0) out vec4 fragmentColor;
layout(location = 1) out vec4 extraOut;

void main () {
    fragmentColor = texture(baseImage, vertexIn.texCoords);
    fragmentColor.rgb *= vertexIn.color;
    fragmentColor.a *= vertexIn.alpha;
    
    extraOut = fragmentColor;
}