#version 330
layout(location = 0) in vec3 vertex;
uniform mat4 projection;
uniform mat4 modelview;

void main(void) {
    gl_Position = modelview * vec4(vertex, 1.0);
    //gl_Position = vec4(vertex, 1.0);
}
