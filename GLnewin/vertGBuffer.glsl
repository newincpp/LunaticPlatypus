#version 430 core

layout(location = 0) uniform float uTime;
layout(location = 1) uniform mat4 uCamera;
layout(location = 2) uniform mat4 uMeshTransform;

layout(location = 0) in vec3 vertexPos_;
layout(location = 1) in vec3 vertexNormal_;
layout(location = 2) in vec2 uvCoord_;

layout(location = 0) out vec3 vInfVertexPos_;
layout(location = 1) out vec3 vInfVertexNormal_;
layout(location = 2) out vec2 vInfUvCoord_;
layout(location = 3) out vec2 vInfDepth_;

void main() {    
    vec3 displacement = vec3(sin(uTime * .001), 0.0f, sin(uTime*0.003));
    gl_Position = uCamera * uMeshTransform * vec4(vertexPos_ + displacement, 1.0);

    vInfVertexPos_ = (uMeshTransform * vec4(vertexPos_ + displacement, 1.0)).xyz;
    vInfVertexNormal_ = vertexNormal_;
    vInfUvCoord_ = uvCoord_;
}  
