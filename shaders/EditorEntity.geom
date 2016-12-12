/*
Editor entity geometry shader - Creates rectangles from points and faces them towards player
*/
#version 400

// Input
layout(points) in;
layout(triangle_strip, max_vertices=4) out;

// Uniform matrices.
uniform mat4 viewProjectionMatrix;

// Uniform camera position
uniform vec3 cameraPosition;
uniform vec3 cameraUp;

// Output
out VertexData {
    vec2 texCoords;
} vertexOut;

void main() {
    // Vector from camera to point, cameras position extracted from view matrix
    vec4 look = vec4(cameraPosition - gl_in[0].gl_Position.xyz, 0.0);
    
    // Particle right vector = camera up x look
    vec4 right = normalize(vec4(cross(cameraUp, look.xyz), 0.0)) * 0.5;
    
    // Particle up vector = particle right x look
    vec4 up = normalize(vec4(cross(look.xyz, right.xyz), 0.0)) * 0.5;
    
    vec4 worldPosition = gl_in[0].gl_Position - right + up;
    gl_Position = viewProjectionMatrix * worldPosition;
    vertexOut.texCoords = vec2(0.0, 0.0);
    EmitVertex();
    
    worldPosition = gl_in[0].gl_Position - right - up;
    gl_Position = viewProjectionMatrix * worldPosition;
    vertexOut.texCoords = vec2(1.0, 0.0);
    EmitVertex();
    
    worldPosition = gl_in[0].gl_Position + right + up;
    gl_Position = viewProjectionMatrix * worldPosition;
    vertexOut.texCoords = vec2(0.0, 1.0);
    EmitVertex();
    
    worldPosition = gl_in[0].gl_Position + right - up;
    gl_Position = viewProjectionMatrix * worldPosition;
    vertexOut.texCoords = vec2(1.0, 1.0);
    EmitVertex();
    
    EndPrimitive();
}
