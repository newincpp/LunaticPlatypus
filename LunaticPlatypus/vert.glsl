#version 410 core

uniform float uTime;
uniform mat4 uCamera;
uniform mat4 uMeshTransform;

layout(location = 0)in vec3  vertexPos_;
layout(location = 1)in vec3  vertexNormal_;
layout(location = 2)in vec2  uvCoord_;

out vec3 frag;

void main() {
    vec3 displacement = vec3(sin(uTime * .001), 0.0f, sin(uTime*0.003)) * 2.0f;
    frag = vec3(vertexNormal_);
    gl_Position = uCamera * uMeshTransform * vec4(vertexPos_ + displacement, 1.0);
}
