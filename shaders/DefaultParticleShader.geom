#version 430 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

out vec2 UV;
in vec4 myColor[];
out vec4 finalColor;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform int textureIndex;
uniform float textureAtlasRows;
uniform float scale;

void main(void) {
    float atlasColumn = mod(textureIndex, textureAtlasRows);
    float atlasRow = floor(textureIndex/textureAtlasRows);
    
    float atlasIndexOffset = 1.0/textureAtlasRows;
    vec2 atlasCoordOffset = vec2(atlasColumn*atlasIndexOffset, atlasRow*atlasIndexOffset);
    
    int i = 0;
    int points = 0;
	
	vec4 right = vec4(viewMatrix[0][0], 
                    viewMatrix[1][0], 
                    viewMatrix[2][0], 0.0);

	vec4 up = vec4(viewMatrix[0][1], 
                 viewMatrix[1][1], 
                 viewMatrix[2][1], 0.0);
    
    finalColor = myColor[i];
	
	gl_Position = (gl_in[i].gl_Position - (right + up) * scale);
	gl_Position = projMatrix * viewMatrix * gl_Position;
	UV = vec2(atlasCoordOffset.x + atlasIndexOffset, atlasCoordOffset.y);
    EmitVertex();
	
	gl_Position = (gl_in[i].gl_Position + (right - up) * scale);
	gl_Position = projMatrix * viewMatrix * gl_Position;
    UV = vec2(atlasCoordOffset.x, atlasCoordOffset.y);
    EmitVertex();
    
    gl_Position = (gl_in[i].gl_Position - (right - up) * scale);
	gl_Position = projMatrix * viewMatrix * gl_Position;
	UV = vec2(atlasCoordOffset.x+atlasIndexOffset, atlasCoordOffset.y+atlasIndexOffset);
    EmitVertex();
    
    gl_Position = (gl_in[i].gl_Position + (right + up) * scale);
	gl_Position = projMatrix * viewMatrix * gl_Position;
	UV = vec2(atlasCoordOffset.x, atlasCoordOffset.y + atlasIndexOffset);
    EmitVertex();
    
    EndPrimitive();
}