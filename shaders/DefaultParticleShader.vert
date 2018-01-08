#version 400

layout (location = 0) in vec4 position; 
layout (location = 1) in vec4 color;

out vec4 myColor;

void main()
{
    gl_Position = vec4(position.xyz, 1.0);
    
    myColor = color;
	
}
