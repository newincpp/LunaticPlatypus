#version 330
uniform float color;

void main(void) {
    gl_FragColor = vec4(0.0,0.3,color, 1.0);
}
