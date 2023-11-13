/*
Index format test.
*/
precision highp float;
precision highp int;

/*const vec2 positions[4] = vec2[](
    vec2(0.01, 0.98),
    vec2(0.99, -0.98),
    vec2(0.99, 0.98),
    vec2(0.01, -0.98)
);*/

MATRICES {
    vec2 positions[4];
} matrices;

void main () {
    if (VertexIndex > 3u) {
        gl_Position = vec4(-0.99, -0.98, 0.0, 1.0);
    } else {
        gl_Position = vec4(matrices.positions[VertexIndex], 0.0, 1.0);
    }
    
    FixPosition();
}
