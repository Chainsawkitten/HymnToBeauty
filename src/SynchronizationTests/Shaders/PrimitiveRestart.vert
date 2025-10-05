/*
Index format test.
*/
precision highp float;
precision highp int;

MATRICES {
    vec2 positions[4];
} matrices;

void main () {
    if (VertexIndex == 0u) {
        gl_Position = vec4(0.01, 0.98, 0.0, 1.0);
    } else if (VertexIndex == 1u) {
        gl_Position = vec4(0.99, -0.98, 0.0, 1.0);
    } else if (VertexIndex == 2u) {
        gl_Position = vec4(0.99, 0.98, 0.0, 1.0);
    } else if (VertexIndex == 3u) {
        gl_Position = vec4(0.01, -0.98, 0.0, 1.0);
    } else {
        gl_Position = vec4(-0.99, -0.98, 0.0, 1.0);
    }
    
    FixPosition();
}
