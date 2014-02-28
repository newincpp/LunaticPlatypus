#version 330
layout(location = 0) in vec3 vertex;
uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 object;

void main(void) {
    gl_Position = projection * modelview * object * vec4(vertex, 1.0);
}
