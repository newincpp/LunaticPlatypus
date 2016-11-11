#version 410 core

uniform float uTime;
//uniform mat4 uCamera;
//uniform mat4 uMeshTransform;

in vec3 frag;

out vec4 outColour;

void main() {
    outColour = vec4(frag, 0.0f);
//    outColour = vec4(sin(uTime), sin(uTime), sin(uTime*10), 1.0);
}
