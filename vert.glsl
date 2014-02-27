#version 330

layout(location = 0) in vec4 vertex;
uniform vec4 displacement;

void main(void) {
    vec4 vertexPosition = vertex;
    vertexPosition += displacement;
    gl_Position = vertexPosition;
}
