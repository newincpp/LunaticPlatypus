#version 330
in vec4 vertexPosition;
uniform float color;

void main(void) {
    //gl_FragColor = vertexPosition;
    gl_FragColor = vec4(0.0,0.3,color, 1.0);
}
