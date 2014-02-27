#version 330

layout(location = 0) in vec4 vertex;
out vec4 vertexPosition;

void main(void) {
    vertexPosition = vertex;
    vertexPosition += vec4(0.1,0.1,0.1, 0.1);
    gl_Position = vertex;
}
