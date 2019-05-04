layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec2 Frag_UV;
layout(location = 1) out vec4 Frag_Color;

PUSH_CONSTANTS
{
    mat4 projectionMatrix;
} pushConst;

void main() {
    Frag_UV = inUV;
    Frag_Color = inColor;
    gl_Position = pushConst.projectionMatrix * vec4(inPosition.xy, 0, 1);
    
    FixPosition();
}
