#version 430 core

layout (location = 0) uniform float uTime;
//layout (location = 1) uniform mat4 uCamera;
layout (location = 2) uniform mat4 uMeshTransform;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 vertexNormal_;
layout (location = 2) in vec2 texCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;

out vec2 TexCoords;

void main() {
    gl_Position = uMeshTransform * vec4(position.x, position.y, 0.0f, 1.0f);
    TexCoords = texCoords;
}  
